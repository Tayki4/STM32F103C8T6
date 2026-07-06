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

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t x = 0;
int y = 0;


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


CAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8];
uint32_t TxMailbox;

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
static void MX_USART3_UART_Init(void);
static void MX_CAN_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_USART3_UART_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  
  /* Tam 2 Byte'lık veri paketi beklediğimizi belirtiyoruz */
  HAL_UART_Receive_IT(&huart3, rx_data, 2);

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

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* GDB'den değiştirilen pwm_duty değerini TIM2 CH4'e yaz */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, pwm_duty);
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    /* 1. Read Buttons */
    pa9_state  = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);
    pa10_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);
    pc13_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

    /* 2. Direct LED Control (Manual Modes Only) */
    if (led_state == 0) 
    {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); // Force OFF
    }
    else if (led_state == 1) 
    {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);   // Force ON
    }
    /* If led_state == 2, the while loop does nothing to the LED. 
       The TIM2 Interrupt handles it! */


  static uint8_t id_index = 0; // Hangi ID'de olduğumuzu takip eder
    
    uint32_t pseudo_random = HAL_GetTick();

    /* Mesaj Başlığı (Header) Ayarları */
    TxHeader.DLC = 8;                         
    TxHeader.IDE = CAN_ID_STD;                
    TxHeader.RTR = CAN_RTR_DATA;              
    TxHeader.StdId = FIXED_IDS[id_index];  // 4 Sabit ID'den birini seç

    /* Rastgele Veri (Frame) Üretimi */
    TxData[0] = (pseudo_random >> 24) & 0xFF;
    TxData[1] = (pseudo_random >> 16) & 0xFF;
    TxData[2] = (pseudo_random >> 8) & 0xFF;
    TxData[3] = pseudo_random & 0xFF;
    TxData[4] = (pseudo_random * 3) & 0xFF;
    TxData[5] = (pseudo_random * 7) & 0xFF;
    TxData[6] = (pseudo_random * 11) & 0xFF;
    TxData[7] = (pseudo_random * 13) & 0xFF;

    /* Mesajı Gönder */
    HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);

    /* Bir sonraki ID'ye geç (0-1-2-3-0...) */
    id_index++;
    if (id_index >= 4) id_index = 0;

    HAL_Delay(500); // 500 ms bekle


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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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

  /*Configure GPIO pins : PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    /* Gelen 2 Byte'ı birleştirip 16 bitlik sayı elde et:
       (High Byte'ı 8 bit sola kaydır ve Low Byte ile OR işlemine sok) */
    uint16_t temp_duty = (rx_data[0] << 8) | rx_data[1];
    
    /* Sınır koruması (0-1000) */
    if (temp_duty > 1000) temp_duty = 1000;
    
    pwm_duty = temp_duty;
    
    /* Doğrudan donanıma yaz */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, pwm_duty);
    
    /* Sonraki 2 Byte'ı dinlemeye başla */
    HAL_UART_Receive_IT(&huart3, rx_data, 2);
  }
}

/**
  * @brief  CAN RX FIFO 0 Mesaj Geldi Kesmesi
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Gelen mesajı oku ve RxHeader ile RxData değişkenlerine kaydet */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
  {
    /* BİLGİSAYARDAN GELEN VERİ ŞU AN RxData DİZİSİNİN İÇİNDE! */
    
    /* Örnek Kontrol: Eğer bilgisayar 0x555 ID'si ile mesaj gönderirse 
       ve ilk verisi (RxData[0]) 1 ise, LED parlaklığını %100 yap */
    if (RxHeader.StdId == 0x555)
    {
       if (RxData[0] == 1) 
       {
           __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1000); 
       } 
       else if (RxData[0] == 0)
       {
           __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
       }
    }
  }
}


/* USER CODE END 4 */

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
