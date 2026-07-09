# STM32F103C8T6 Embedded Systems Portfolio

Welcome to my embedded software development portfolio! Bu depo (repository), STM32 mikrodenetleyicileri üzerindeki yetkinliğimi, temel GPIO işlemlerinden başlayarak otomotiv endüstrisinde kullanılan karmaşık Gerçek Zamanlı İşletim Sistemlerine (RTOS) kadar giden gelişimimi göstermektedir.

## Dallar (Branches) ve İçerikleri

### 1. `01-Basic-GPIO-Control` (Eski adıyla: main)
Bu dal, gömülü sistemlerin temel yapıtaşlarını içerir.
- **Özellikler:** Temel GPIO yapılandırmaları, butonlardan veri okuma ve LED çıkışlarını kontrol etme.
- **Gösterilen Yetkinlikler:** HAL kütüphaneleri ile donanım ilklendirme, buton arkı (debounce) önleme mantığı ve temel kontrol algoritmaları.

### 2. `02-USART-CAN-Timer-PWM` (Eski adıyla: USART_CAN_TIMER_ADDED)
Bu dal, donanım zamanlayıcılarını (Timer), PWM sinyallerini ve endüstriyel haberleşme protokollerini işin içine katarak projeyi daha ileri bir seviyeye taşır.
- **Özellikler:**
  - LED parlaklığını kontrol etmek için Donanımsal Timer (TIM2) ile PWM sinyali üretimi.
  - Kesme (Interrupt - IT) tabanlı USART haberleşmesi sayesinde sistemi bloklamadan (non-blocking) veri alma ve gönderme.
  - Filtreleme destekli CAN Bus protokolü entegrasyonu (Kesme ile RX/TX).
- **Gösterilen Yetkinlikler:** Kesme (Interrupt) yönetimi, asenkron haberleşme, donanım seviyesinde PWM kontrolü ve otomotiv standartlarında CAN Bus protokolü yapılandırması.

### 3. `03-FreeRTOS-Automotive` (Eski adıyla: Last - Aktif Dal)
Bu dal, modern otomotiv ECU'larında (Elektronik Kontrol Ünitesi) kullanılan mimarilere çok benzer bir Gerçek Zamanlı İşletim Sistemi (CMSIS-RTOS / FreeRTOS) uygulaması sunarak ileri düzey gömülü yazılım mühendisliği yeteneklerimi sergiler.
- **Özellikler:**
  - Çoklu görev (Multi-tasking) yapısı (`LEDTask`, `CANTask`, `UARTTask`).
  - Güvenilir görev zamanlaması için RTOS tabanlı bayrak (flag) mekanizmaları ve olay güdümlü (event-driven) mimari.
  - CAN Bus mesajlaşması ve UART seri haberleşmesinin ana işlemciyi (CPU) meşgul etmeden eşzamanlı ve paralel olarak yürütülmesi.
  - İleri düzey Mutex, Semafor (Semaphore) ve donanım zamanlayıcı (Timer) altyapısı.
- **Gösterilen Yetkinlikler:** İş parçacığı güvenliği (thread-safe) sağlanan, ölçeklenebilir gerçek zamanlı sistemler tasarlama, görev senkronizasyonu ve otomotiv standartlarına uygun eşzamanlı (concurrent) yazılım mimarisi geliştirme.

## Geliştirici Hakkında
Bu dallar arasındaki gelişim; C programlama diline, STM32 HAL ile LL (Low-Level) sürücülerine ve RTOS prensiplerine olan derin hakimiyetimi vurgulamaktadır. Gerçek dünya otomotiv sistemlerinin katı zamanlama (strict timing) ve yüksek güvenilirlik (reliability) gereksinimlerini karşılayabilecek, ölçeklenebilir gömülü yazılımlar tasarlama konusunda yetkinim.
