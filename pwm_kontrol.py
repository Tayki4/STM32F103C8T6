import tkinter as tk
import serial

# --- AYARLAR ---
PORT = "COM5"  # Kendi portunu buraya yaz
BAUD = 115200

# Seri portu açmayı dene
try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"{PORT} portuna başarıyla bağlanıldı!")
except Exception as e:
    ser = None
    print(f"Bağlantı Hatası: Port açılamadı. Hercules'in kapalı olduğundan emin ol!\n{e}")

# --- HABERLEŞME FONKSİYONU ---
def parlaklik_gonder(deger):
    """Slider hareket ettikçe bu fonksiyon tetiklenir."""
    if ser and ser.is_open:
        # Kaydırma çubuğundaki sayıyı (Örn: 1000) integer'a çevir
        sayi = int(deger)
        
        # Sayıyı 2 byte'lık ham veriye (Raw Bytes) çevir (Big Endian formatında)
        # Örn: 1000 sayısı -> 0x03 ve 0xE8 olarak iki bayta bölünür
        ham_veri = sayi.to_bytes(2, byteorder='big')
        
        # Doğrudan bitleri (byte'ları) porttan yolla (encode etmeye gerek yok)
        ser.write(ham_veri)
        
# --- ARAYÜZ TASARIMI ---
pencere = tk.Tk()
pencere.title("STM32 PWM Kontrol Paneli")
pencere.geometry("350x150")

# Başlık
tk.Label(pencere, text="LED Parlaklığı (0 - 1000)", font=("Arial", 12, "bold")).pack(pady=15)

# Kaydırma Çubuğu (Slider)
slider = tk.Scale(pencere, from_=0, to=1000, orient="horizontal", length=280, command=parlaklik_gonder)
slider.set(0) # Başlangıçta 0'da dursun
slider.pack()

pencere.mainloop()