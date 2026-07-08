import tkinter as tk
import can

# --- 1. PCAN BAĞLANTISINI KUR ---
try:
    bus = can.interface.Bus(interface='pcan', channel='PCAN_USBBUS1', bitrate=250000)
    baglanti_durumu = "✅ PCAN Bağlantısı Başarılı"
except Exception as e:
    bus = None
    baglanti_durumu = "❌ PCAN Hatası! (PCAN-View Kapalı mı?)"
    print("Hata Detayı:", e)

# Küresel değişken (LED'in durumunu takip etmek için)
led_acik_mi = False

# --- 2. SLIDER HAREKET ETTİKÇE ÇALIŞACAK FONKSİYON ---
def parlaklik_gonder(val):
    global led_acik_mi
    
    deger = int(val)
    
    # PCAN bağlıysa CAN mesajını gönder
    if bus is not None:
        veri = [(deger >> 8) & 0xFF, deger & 0xFF]
        msg = can.Message(arbitration_id=0x555, data=veri, is_extended_id=False)
        try:
            bus.send(msg)
        except can.CanError:
            print("Mesaj CAN hattına gönderilemedi!")
            
    # Arayüz Senkronizasyonu (Slider'a göre butonu güncelle)
    if deger == 0:
        btn_ac_kapat.config(text="Tam Güç Aç", bg="green", fg="white")
        led_acik_mi = False
    elif deger == 1000:
        btn_ac_kapat.config(text="Tamamen Kapat", bg="red", fg="white")
        led_acik_mi = True
    else:
        # Ara değerlerdeyken butonu kırmızı yapıp kapatmaya hazır hale getir
        btn_ac_kapat.config(text="Tamamen Kapat", bg="red", fg="white")
        led_acik_mi = True

# --- 3. BUTON ÇALIŞACAK FONKSİYON ---
def led_ac_kapat():
    if led_acik_mi:
        # Açıksa tamamen kapat -> Slider'ı 0'a çek 
        # (Bu işlem otomatik olarak parlaklik_gonder(0) fonksiyonunu tetikler)
        slider.set(0)
    else:
        # Kapalıysa tam güç aç -> Slider'ı 1000'e çek
        # (Bu işlem otomatik olarak parlaklik_gonder(1000) fonksiyonunu tetikler)
        slider.set(1000)

# --- 4. GÖRSEL ARAYÜZ (GUI) TASARIMI ---
root = tk.Tk()
root.title("CAN Bus - STM32 Kontrol Merkezi")
root.geometry("380x260") # Buton sığsın diye pencereyi biraz uzattık
root.configure(padx=20, pady=20)

# Durum Bildirim Etiketi
durum_label = tk.Label(root, text=baglanti_durumu, font=("Arial", 10, "bold"), 
                       fg="green" if bus else "red")
durum_label.pack(pady=5)

# Başlık Etiketi
baslik_label = tk.Label(root, text="Kırmızı LED Parlaklığı (PA3)", font=("Arial", 12))
baslik_label.pack(pady=5)

# Kaydırma Çubuğu (Slider)
slider = tk.Scale(root, from_=0, to=1000, orient=tk.HORIZONTAL, length=300, 
                  command=parlaklik_gonder, tickinterval=250)
slider.pack(pady=10)

# Aç/Kapat Butonu
btn_ac_kapat = tk.Button(root, text="Tam Güç Aç", font=("Arial", 10, "bold"), 
                         bg="green", fg="white", width=15, height=2, 
                         command=led_ac_kapat)
btn_ac_kapat.pack(pady=5)

# Arayüzü Başlat
root.mainloop()