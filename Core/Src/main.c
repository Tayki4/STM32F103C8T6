/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "stm32f1xx_ll_tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

/* osThreadId defaultTaskHandle; (Gereksiz olduğu için silindi) */
/* USER CODE BEGIN PV */
extern volatile uint32_t timer_tick;
osThreadId CANTaskHandle;
osThreadId UARTTaskHandle;
osMessageQId usartQueueHandle;
osMessageQId canQueueHandle;
uint8_t x = 0;
int y = 0;

/* Custom Tick Variable for legacy code (if any) */
volatile uint32_t my_tick = 0;


/* PWM Duty Cycle for LED brightness on PA3
   0    = LED OFF
   500  = %50 Brightness
   1000 = %100 Brightness
*/
volatile int pwm_duty = 0; 
uint8_t rx_data[2]; // Sadece 2 byte (16 bit) ham veri tutacak

/* Button states */
GPIO_PinState pa9_state;
GPIO_PinState pa10_state;
GPIO_PinState pc13_state;

volatile int led_state = 2; 

/* CHANGED to 'volatile int' for perfect GDB compatibility. */
/* UART Haberleşme Değişkenleri */

uint8_t rx_byte;          // Gelen tek bir karakteri tutar
char rx_buffer[10];       // Gelen mesajı biriktirdiğimiz dizi (Örn: "500")
uint8_t rx_index = 0;     // Dizinin neresinde olduğumuzu tutar

CAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8];    // 8 Byte'lık CAN veri çerçevesi (Frame)
uint32_t TxMailbox;   // Verinin yollanacağı posta kutusu


/* Bilgisayardan gelen verileri okumak için gereken değişkenler */
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

/* 4 Adet Sabit CAN ID Tanımlaması (Standart 11-bit ID'ler) */
const uint32_t FIXED_IDS[4] = {0x10A, 0x20B, 0x30C, 0x40D};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_CAN_Init(void);
static void MX_USART1_UART_Init(void);
/* void StartDefaultTask(void const * argument); (Silindi) */

/* USER CODE BEGIN PFP */
void USART1_SendChar(char c);
void USART1_SendString(const char *str);
void StartCANTask(void const * argument);
void StartUARTTask(void const * argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  
  /* 0 = LED OFF, 1 = LED ON */
  
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    /* TIM2 PWM ve zamanlayıcı ayarları (LL kullanarak) */
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH4);
    LL_TIM_ClearFlag_UPDATE(TIM2);
    LL_TIM_EnableIT_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);
  
  /* USART1 RXNE kesmesini aktif et */
  LL_USART_EnableIT_RXNE(USART1);

/* CAN Filtre Ayarları (Tüm ID'leri kabul edecek şekilde varsayılan ayar) */
  CAN_FilterTypeDef canfilterconfig;
  canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
  canfilterconfig.FilterBank = 0;
  canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
  canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
  canfilterconfig.FilterIdHigh = 0x0000;
  canfilterconfig.FilterIdLow = 0x0000;
  canfilterconfig.FilterMaskIdHigh = 0x0000;
  canfilterconfig.FilterMaskIdLow = 0x0000;
  canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  HAL_CAN_ConfigFilter(&hcan, &canfilterconfig);

  /* CAN Donanımını Başlat */
  HAL_CAN_Start(&hcan);

/* 2. CAN RX (Okuma) Kesmesini Aktif Et (BİLGİSAYARDAN VERİ ALMAK İÇİN KRİTİK) */
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

  /* USER CODE END 2 */

// ****tick kullanmaya TEVBE et
// timer'ı 1 ms periyoda ya da 1kHz frekansa ayarla, ctr'ı ms ctr oarak kullanabilsin
// urat send char fonktaki blocking while yerine timeout!!!
// kullanılmayan rtos tasklar iptal.
// gerekirse timer interrupt'ı ve rtos task priority sıralamasını cubemxten güncelle.

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  osMessageQDef(usartQueue, 16, uint16_t);
  usartQueueHandle = osMessageCreate(osMessageQ(usartQueue), NULL);

  osMessageQDef(canQueue, 16, uint16_t);
  canQueueHandle = osMessageCreate(osMessageQ(canQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* defaultTask gereksiz olduğu için silindi. Toplam task sayısı düşürüldü. */

  /* USER CODE BEGIN RTOS_THREADS */
  /* definition and creation of CANTask */
  osThreadDef(CANTask, StartCANTask, osPriorityNormal, 0, 128);
  CANTaskHandle = osThreadCreate(osThread(CANTask), NULL);

  /* definition and creation of UARTTask */
  osThreadDef(UARTTask, StartUARTTask, osPriorityNormal, 0, 128);
  UARTTaskHandle = osThreadCreate(osThread(UARTTask), NULL);
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 9;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_12TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  /* TIM2 interrupt Init */
  NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(TIM2_IRQn);

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  TIM_InitStruct.Prescaler = 71;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 999;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH4);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 0;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH4);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**TIM2 GPIO Configuration
  PA3   ------> TIM2_CH4
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration
  PA9   ------> USART1_TX
  PA10   ------> USART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_Enable(USART1);
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void USART1_SendChar(char c)
{
  uint32_t timeout = 0xFFFF;
  while (!LL_USART_IsActiveFlag_TXE(USART1) && timeout > 0)
  {
    timeout--;
  }
  if (timeout > 0)
  {
    LL_USART_TransmitData8(USART1, c);
  }
}

void USART1_SendString(const char *str)
{
  while (*str)
  {
    USART1_SendChar(*str++);
  }
}

/* HAL CAN RX Callback (Artık kullanılmıyor, Bare-Metal ISR devrede) */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
}

/* FreeRTOS Görev Tanımlamaları */
void StartCANTask(void const * argument)
{
  uint32_t random_val = 0;
  uint8_t id_index = 0;
  
  uint32_t last_can_tx_tick = timer_tick;
  
  for(;;)
  {
    /* 1. Olay Tabanlı CAN Veri Bekleme (Sadece 1ms uyuma payı, FreeRTOS Timeout yerine) */
    osEvent evt = osMessageGet(canQueueHandle, 1);
    
    if (evt.status == osEventMessage)
    {
      uint16_t temp_duty = evt.value.v;
      if (temp_duty > 1000) temp_duty = 1000;
      pwm_duty = temp_duty;
      
      /* LED parlaklığını güncelle (LL kullanılarak) */
      LL_TIM_OC_SetCompareCH4(TIM2, pwm_duty);
    }

    /* 2. BARE-METAL CAN GÖNDERİMİ (Her 50 ms'de bir, Timer üzerinden) */
    if ((timer_tick - last_can_tx_tick) >= 50)
    {
      last_can_tx_tick = timer_tick;
      random_val++;

      // 8 Byte veriyi 2 adet 32-bit register (TDLR, TDHR) için paketle
      uint32_t tdlr = (random_val & 0xFF) | (((random_val >> 8) & 0xFF) << 8) | (((random_val >> 16) & 0xFF) << 16) | (((random_val >> 24) & 0xFF) << 24);
      uint32_t tdhr = ((random_val * 3) & 0xFF) | (((random_val * 7) & 0xFF) << 8) | (((random_val * 11) & 0xFF) << 16) | (((random_val * 13) & 0xFF) << 24);

      // Boş bir posta kutusu (Mailbox 0) var mı diye kontrol et (TME0 biti)
      if ((CAN1->TSR & CAN_TSR_TME0) == CAN_TSR_TME0)
      {
        CAN1->sTxMailBox[0].TDLR = tdlr;
        CAN1->sTxMailBox[0].TDHR = tdhr;
        CAN1->sTxMailBox[0].TDTR = 8; // DLC = 8
        
        // Standart ID'yi yerleştir ve gönderimi (TXRQ) başlat
        CAN1->sTxMailBox[0].TIR = (FIXED_IDS[id_index] << 21) | CAN_TI0R_TXRQ;
      }

      id_index++;
      if (id_index >= 4) id_index = 0;
    }
  }
}

void StartUARTTask(void const * argument)
{
  uint32_t last_uart_tx_tick = timer_tick;
  for(;;)
  {
    /* 1. Olay Tabanlı USART Veri Bekleme */
    osEvent evt = osMessageGet(usartQueueHandle, 1);
    
    if (evt.status == osEventMessage)
    {
      uint16_t temp_duty = evt.value.v;
      if (temp_duty > 1000) temp_duty = 1000;
      pwm_duty = temp_duty;
      
      /* LED parlaklığını güncelle (LL kullanılarak) */
      LL_TIM_OC_SetCompareCH4(TIM2, pwm_duty);
    }

    /* 2. YEŞİL LED (PB12) GÜNCELLEMESİ (LL kullanılarak) */
    if (led_state == 0) 
    {
      LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
    }
    else if (led_state == 1) 
    {
      LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
    }

    /* 3. USART1 PERİYODİK BİLGİ GÖNDERİMİ (Timer üzerinden 5 saniyede bir) */
    if ((timer_tick - last_uart_tx_tick) >= 5000)
    {
      last_uart_tx_tick = timer_tick;
      USART1_SendString("taylan buradaydi.\r\n");
    }
  }
}
/* SysTick'i devre dışı bırakmak için HAL tick ilklendirmesini eziyoruz */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /* SysTick'i başlatmıyoruz, TIM2 kesmesi üzerinden zaman takibi yapılacaktır */
  return HAL_OK;
}

/* FreeRTOS'un SysTick başlatma fonksiyonunu ezerek SysTick kullanımını engelliyoruz */
void vPortSetupTimerInterrupt(void)
{
  /* TIM2 zaten main.c içinde başlatıldığı için burada işlem yapmıyoruz */
}
/* USER CODE END 4 */

/* defaultTask tamamen kaldırıldı, sistem boşuna yorulmayacak. */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
