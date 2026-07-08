import tkinter as tk
import tkinter.scrolledtext as st
import can

# --- 1. PCAN BAĞLANTISINI KUR ---
try:
    bus = can.interface.Bus(interface='pcan', channel='PCAN_USBBUS1', bitrate=250000)
    baglanti_durumu = "✅ PCAN Bağlantısı Başarılı"
except Exception as e:
    bus = None
    baglanti_durumu = "❌ PCAN Hatası! (PCAN-View Kapalı mı?)"
    print("Hata Detayı:", e)

led_acik_mi = False

# --- 2. CAN HATTINI DİNLEME (ARKA PLAN) ---
def can_mesajlarini_dinle():
    if bus is not None:
        # Kuyruktaki tüm bekleyen mesajları hiç beklemeden (0.0 sn) hızlıca al
        while True:
            msg = bus.recv(0.0)
            if msg is None:
                break # Okunacak yeni mesaj kalmadıysa döngüden çık
            
            # Sadece kendi gönderdiğimiz (0x555) mesajları ekranda görmezden gel
            if msg.arbitration_id != 0x555:
                id_hex = hex(msg.arbitration_id).upper()
                veri_hex = " ".join(f"{b:02X}" for b in msg.data)
                
                # Ekrana formatlı şekilde yazdır
                satir = f"ID: {id_hex} | Veri: {veri_hex}\n"
                gelen_kutusu.insert(tk.END, satir)
                gelen_kutusu.see(tk.END) # Otomatik olarak en alta kaydır
                
                # Kutu çok dolup bilgisayarı kastırmasın diye ilk satırları sil (Son 50 mesajı tut)
                if int(gelen_kutusu.index('end-1c').split('.')[0]) > 50:
                    gelen_kutusu.delete('1.0', '2.0')

    # Bu fonksiyonu 20 milisaniye sonra arka planda tekrar çağır
    root.after(20, can_mesajlarini_dinle)

# --- 3. GÖNDERME FONKSİYONLARI ---
def parlaklik_gonder(val):
    global led_acik_mi
    deger = int(val)
    
    if bus is not None:
        veri = [(deger >> 8) & 0xFF, deger & 0xFF]
        msg = can.Message(arbitration_id=0x555, data=veri, is_extended_id=False)
        try:
            bus.send(msg)
        except can.CanError:
            pass
            
    if deger == 0:
        btn_ac_kapat.config(text="Tam Güç Aç", bg="green", fg="white")
        led_acik_mi = False
    elif deger == 1000:
        btn_ac_kapat.config(text="Tamamen Kapat", bg="red", fg="white")
        led_acik_mi = True
    else:
        btn_ac_kapat.config(text="Tamamen Kapat", bg="red", fg="white")
        led_acik_mi = True

def led_ac_kapat():
    if led_acik_mi:
        slider.set(0)
    else:
        slider.set(1000)

# --- 4. GÖRSEL ARAYÜZ (GUI) TASARIMI ---
root = tk.Tk()
root.title("CAN Bus - STM32 Kontrol Merkezi")
root.geometry("450x450") # Yeni penceremiz artık daha büyük
root.configure(padx=20, pady=10)

# Durum Bildirim Etiketi
durum_label = tk.Label(root, text=baglanti_durumu, font=("Arial", 10, "bold"), 
                       fg="green" if bus else "red")
durum_label.pack(pady=5)

# --- KONTROL BÖLÜMÜ ---
frame_kontrol = tk.LabelFrame(root, text=" İletim (TX) - LED Kontrol ", padx=10, pady=10)
frame_kontrol.pack(fill="x", pady=10)

slider = tk.Scale(frame_kontrol, from_=0, to=1000, orient=tk.HORIZONTAL, length=300, 
                  command=parlaklik_gonder, tickinterval=250)
slider.pack()

btn_ac_kapat = tk.Button(frame_kontrol, text="Tam Güç Aç", font=("Arial", 10, "bold"), 
                         bg="green", fg="white", width=15, height=1, 
                         command=led_ac_kapat)
btn_ac_kapat.pack(pady=5)

# --- DİNLEME BÖLÜMÜ ---
frame_dinleme = tk.LabelFrame(root, text=" Alım (RX) - Gelen CAN Mesajları ", padx=10, pady=10)
frame_dinleme.pack(fill="both", expand=True)

# Otomatik kaydırmalı (ScrolledText) metin kutusu
gelen_kutusu = st.ScrolledText(frame_dinleme, width=40, height=10, font=("Courier New", 9))
gelen_kutusu.pack(fill="both", expand=True)

# Dinleme döngüsünü başlat (Bu satır sihrin başladığı yerdir)
root.after(20, can_mesajlarini_dinle)

# Arayüzü Başlat
root.mainloop()