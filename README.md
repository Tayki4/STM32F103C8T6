# STM32F103C8T6 Embedded Systems Portfolio

Welcome to my embedded software development portfolio! This repository demonstrates my progression and expertise in programming STM32 microcontrollers, advancing from basic GPIO operations to complex, real-time operating systems (RTOS) used in the automotive industry.

## Branches Overview

### 1. `01-Basic-GPIO-Control`
This branch contains foundational embedded system concepts.
- **Features:** Basic GPIO configurations, reading inputs from push buttons, and controlling LED outputs.
- **Skills Demonstrated:** Bare-metal-like peripheral initialization using HAL, button debouncing, and basic control logic.

### 2. `02-USART-CAN-Timer-PWM`
This branch steps up the complexity by introducing hardware timers, PWM signals, and industrial communication protocols.
- **Features:**
  - Hardware Timers (TIM2) configured for PWM signal generation to control LED brightness.
  - Interrupt (IT) based USART communication for non-blocking data reception and transmission.
  - CAN Bus protocol integration (RX/TX via interrupts) with message filtering.
- **Skills Demonstrated:** Interrupt handling, asynchronous communication, hardware-level PWM control, and automotive-standard CAN protocol configuration.

### 3. `03-FreeRTOS-Automotive` (Active Branch)
This branch showcases advanced, real-world embedded software engineering by implementing a Real-Time Operating System (CMSIS-RTOS/FreeRTOS). The architecture is highly relevant to modern automotive ECUs (Electronic Control Units).
- **Features:**
  - Multi-tasking environment (`LEDTask`, `CANTask`, `UARTTask`).
  - RTOS-based flag mechanisms and event-driven architecture for robust scheduling.
  - Concurrent handling of CAN Bus messaging and UART serial communication without blocking the main CPU execution.
  - Advanced Mutex, Semaphore, and hardware Timer structures.
- **Skills Demonstrated:** Designing thread-safe, scalable real-time systems, task synchronization, scheduling, and automotive-grade concurrent software architecture.

## About the Developer
The evolution across these branches highlights my strong command over C programming, STM32 HAL libraries, Low-Level (LL) drivers, and RTOS principles. I am adept at designing scalable embedded software that meets the strict timing and reliability requirements of real-world automotive systems.

---

# STM32F103C8T6 Gömülü Sistemler Portfolyosu

Gömülü yazılım geliştirme portfolyoma hoş geldiniz! Bu depo, STM32 mikrodenetleyicileri üzerindeki yetkinliğimi, temel GPIO işlemlerinden başlayarak otomotiv endüstrisinde kullanılan karmaşık Gerçek Zamanlı İşletim Sistemlerine (RTOS) kadar giden gelişimimi göstermektedir.

## Dallar (Branches) ve İçerikleri

### 1. `01-Basic-GPIO-Control`
Bu dal, gömülü sistemlerin temel yapıtaşlarını içerir.
- **Özellikler:** Temel GPIO yapılandırmaları, butonlardan veri okuma ve LED çıkışlarını kontrol etme.
- **Gösterilen Yetkinlikler:** HAL kütüphaneleri ile donanım ilklendirme, buton arkı (debounce) önleme mantığı ve temel kontrol algoritmaları.

### 2. `02-USART-CAN-Timer-PWM`
Bu dal, donanım zamanlayıcılarını (Timer), PWM sinyallerini ve endüstriyel haberleşme protokollerini işin içine katarak projeyi daha ileri bir seviyeye taşır.
- **Özellikler:**
  - LED parlaklığını kontrol etmek için Donanımsal Timer (TIM2) ile PWM sinyali üretimi.
  - Kesme (Interrupt - IT) tabanlı USART haberleşmesi sayesinde sistemi bloklamadan (non-blocking) veri alma ve gönderme.
  - Filtreleme destekli CAN Bus protokolü entegrasyonu (Kesme ile RX/TX).
- **Gösterilen Yetkinlikler:** Kesme (Interrupt) yönetimi, asenkron haberleşme, donanım seviyesinde PWM kontrolü ve otomotiv standartlarında CAN Bus protokolü yapılandırması.

### 3. `03-FreeRTOS-Automotive` (Aktif Dal)
Bu dal, modern otomotiv ECU'larında (Elektronik Kontrol Ünitesi) kullanılan mimarilere çok benzer bir Gerçek Zamanlı İşletim Sistemi (CMSIS-RTOS / FreeRTOS) uygulaması sunarak ileri düzey gömülü yazılım mühendisliği yeteneklerimi sergiler.
- **Özellikler:**
  - Çoklu görev (Multi-tasking) yapısı (`LEDTask`, `CANTask`, `UARTTask`).
  - Güvenilir görev zamanlaması için RTOS tabanlı bayrak (flag) mekanizmaları ve olay güdümlü (event-driven) mimari.
  - CAN Bus mesajlaşması ve UART seri haberleşmesinin ana işlemciyi (CPU) meşgul etmeden eşzamanlı ve paralel olarak yürütülmesi.
  - İleri düzey Mutex, Semafor (Semaphore) ve donanım zamanlayıcı (Timer) altyapısı.
- **Gösterilen Yetkinlikler:** İş parçacığı güvenliği (thread-safe) sağlanan, ölçeklenebilir gerçek zamanlı sistemler tasarlama, görev senkronizasyonu ve otomotiv standartlarına uygun eşzamanlı (concurrent) yazılım mimarisi geliştirme.

## Geliştirici Hakkında
Bu dallar arasındaki gelişim; C programlama diline, STM32 HAL ile LL (Low-Level) sürücülerine ve RTOS prensiplerine olan derin hakimiyetimi vurgulamaktadır. Gerçek dünya otomotiv sistemlerinin katı zamanlama (strict timing) ve yüksek güvenilirlik (reliability) gereksinimlerini karşılayabilecek, ölçeklenebilir gömülü yazılımlar tasarlama konusunda yetkinim.
