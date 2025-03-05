

#include <Wire.h> // Thư viện Wire cho giao tiếp I2C
#include <DS1307RTC.h> // Thư viện DS1307RTC cho module RTC
#include <LiquidCrystal_I2C.h> // Thư viện LiquidCrystal_I2C cho LCD I2C
#include <TimeLib.h>
#include "DHT.h"

#define DHTPIN 5//Chân cảm biến DHt11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
DS1307RTC rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Khởi tạo đối tượng LCD I2C với địa chỉ 0x3F, 16 cột, 2 dòng

#define Toggle_Up 4 // Định nghĩa chân nút Up
#define Set_time 2 // Định nghĩa chân nút Set time
#define Set_date 3 // Định nghĩa chân nút Set date

bool check_inter_0, check_inter_1; // Biến kiểm tra sự tương tác của nút Up và nút Set time

// Hàm chuyển đổi chuỗi tháng thành số nguyên để xử lý dễ dàng hơn
int Convert_Str_To_Int_Months(int monthIndex) {
  return monthIndex + 1;
}

// Hàm xử lý khi nút Up được nhấn
void Toggle_Up_Value() {
  check_inter_0 = true;
}

// Hàm xử lý khi nút Down được nhấn (chỉ sử dụng cho chỉnh sửa năm)
void Toggle_Down_Value() {
  check_inter_1 = true;
}

// Hàm thiết lập ngày
void set_date_button() {
  tmElements_t tm;
  RTC.read(tm); // Đọc thời gian từ DS1307

  int Test_case = 0;
  int ArrayOfValue[3] = {tm.Day, tm.Month, tmYearToCalendar(tm.Year)}; // Lưu ngày, tháng và năm vào một mảng

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Date:");

  while (Test_case < 3) { // Khi đã chỉnh sửa xong
    if (!digitalRead(Set_date)) {
      while (!digitalRead(Set_date));
      Test_case++;
    }

    lcd.setCursor(3, 1);
    lcd.print(tm.Day);
    lcd.print("/");
    lcd.print(tm.Month);
    lcd.print("/");
    lcd.print(tmYearToCalendar(tm.Year)); // Hiển thị D/M/Y
    delay(500);

    switch (Test_case) { // Nhấp nháy tại vị trí chỉnh sửa
      case 0: {
          lcd.setCursor(3, 1);
          lcd.print("  ");
          delay(500);
          break;
        }

      case 1: {
          lcd.setCursor(6, 1);
          lcd.print("  ");
          delay(500);
          break;
        }

      default: {
          lcd.setCursor(9, 1);
          lcd.print("    ");
          delay(500);
          break;
        }
    }

    lcd.setCursor(3, 1);
    lcd.print(tm.Day);
    lcd.print("/");
    lcd.print(tm.Month);
    lcd.print("/");
    lcd.print(tmYearToCalendar(tm.Year)); // Hiển thị D/M/Y

    if (!digitalRead(Set_date)) {
      while (!digitalRead(Set_date));
      Test_case++;
    }

    if (check_inter_0) {
      while (!digitalRead(Toggle_Up));

      if (Test_case == 0) { // Test_case 0 là ngày, 1 là tháng, 2 là năm
        if (ArrayOfValue[Test_case] == 31)
          ArrayOfValue[Test_case] = 1;
        else
          ArrayOfValue[Test_case]++;
      }

      else if (Test_case == 1) {
        if (ArrayOfValue[Test_case] == 12)
          ArrayOfValue[Test_case] = 1;
        else
          ArrayOfValue[Test_case]++;
      }

      else
        ArrayOfValue[Test_case]++;

      tm.Day = ArrayOfValue[0];
      tm.Month = ArrayOfValue[1];
      tm.Year = CalendarYrToTm(ArrayOfValue[2]);

      RTC.write(tm); // Cập nhật thời gian cho DS1307
      check_inter_0 = false;
    }

    if (Test_case == 2) { // Down (Chỉ sử dụng cho chỉnh sửa năm)
      if (check_inter_1) {
        while (!digitalRead(Set_time));
        ArrayOfValue[Test_case]--;
      }
      tm.Day = ArrayOfValue[0];
      tm.Month = ArrayOfValue[1];
      tm.Year = CalendarYrToTm(ArrayOfValue[2]);

      RTC.write(tm); // Cập nhật thời gian cho DS1307
      check_inter_1 = false;
    }

    if (!digitalRead(Set_date)) {
      while (!digitalRead(Set_date));
      Test_case++;
    }
  }

  lcd.clear();
  lcd.print("Done!");
  delay(500);
  lcd.clear();
}



// Hàm thiết lập thời gian
void set_time_button() {
  tmElements_t tm;
  RTC.read(tm); // Đọc thời gian từ DS1307

  int Test_case = 0;
  int ArrayOfValue[3] = {tm.Hour, tm.Minute, tm.Second}; // Lưu giờ/phút/giây vào một mảng

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Time:");
  lcd.setCursor(4, 1);
  printTime(tm.Hour, tm.Minute, tm.Second);

  while (Test_case < 3) {
    if (!digitalRead(Set_time)) {
      while (!digitalRead(Set_time)) { // Tránh dừng thời gian khi nhấn nút
        lcd.setCursor(4, 1);
        printTime(tm.Hour, tm.Minute, tm.Second);

        RTC.read(tm);
        ArrayOfValue[0] = tm.Hour;
        ArrayOfValue[1] = tm.Minute;
        ArrayOfValue[2] = tm.Second;
      }
      Test_case++;
    }

    lcd.setCursor(4, 1);
    printTime(tm.Hour, tm.Minute, tm.Second);
    delay (500);

    RTC.read(tm); // Cập nhật thời gian
    ArrayOfValue[0] = tm.Hour;
    ArrayOfValue[1] = tm.Minute;
    ArrayOfValue[2] = tm.Second;

    switch (Test_case) { // Nhấp nháy tại vị trí chỉnh sửa
      case 0: {
          lcd.setCursor(4, 1);
          lcd.print("  ");
          delay(500);
          break;
        }

      case 1: {
          lcd.setCursor(7, 1);
          lcd.print("  ");
          delay(500);
          break;
        }

      default: {
          lcd.setCursor(10, 1);
          lcd.print("  ");
          delay(500);
          break;
        }
    }

    lcd.setCursor(4, 1);
    printTime(tm.Hour, tm.Minute, tm.Second);

    if (!digitalRead(Set_time)) { // Pooling
      while (!digitalRead(Set_time)) {
        lcd.setCursor(4, 1);
        printTime(tm.Hour, tm.Minute, tm.Second);

        RTC.read(tm);
        ArrayOfValue[0] = tm.Hour;
        ArrayOfValue[1] = tm.Minute;
        ArrayOfValue[2] = tm.Second;
      }
      Test_case++;
    }



    if (check_inter_0) {
      while (!digitalRead(Toggle_Up)) {
        lcd.setCursor(4, 1); // Tránh dừng thời gian khi nhấn nút
        printTime(tm.Hour, tm.Minute, tm.Second);

        RTC.read(tm);
        ArrayOfValue[0] = tm.Hour;
        ArrayOfValue[1] = tm.Minute;
        ArrayOfValue[2] = tm.Second;
      }

      if (Test_case == 0) { // Đặt giờ
        if (ArrayOfValue[Test_case] == 23)
          ArrayOfValue[Test_case] = 0;
        else
          ArrayOfValue[Test_case]++;
      }

      else if (Test_case == 1) { // Đặt phút
        if (ArrayOfValue[Test_case] == 59)
          ArrayOfValue[Test_case] = 0;
        else
          ArrayOfValue[Test_case]++;
      }

      else // Đặt giây
        ArrayOfValue[Test_case] = 0;

      tm.Hour = ArrayOfValue[0];
      tm.Minute = ArrayOfValue[1];
      tm.Second = ArrayOfValue[2];

      RTC.write(tm); // Cập nhật thời gian cho DS1307
      check_inter_0 = false;
    }

    RTC.read(tm); // Cập nhật thời gian từ DS1307
    ArrayOfValue[0] = tm.Hour;
    ArrayOfValue[1] = tm.Minute;
    ArrayOfValue[2] = tm.Second;

    if (!digitalRead(Set_time)) { // Pooling
      while (!digitalRead(Set_time)) {
        lcd.setCursor(4, 1);
        printTime(tm.Hour, tm.Minute, tm.Second);

        RTC.read(tm);
        ArrayOfValue[0] = tm.Hour;
        ArrayOfValue[1] = tm.Minute;
        ArrayOfValue[2] = tm.Second;
      }
      Test_case++;
    }
  }

  lcd.clear();
  lcd.print("Done!");
  delay(500);
  lcd.clear();
}

// Hàm in ra thời gian
void printTime(int hour, int minute, int second) {
  if (hour < 10) lcd.print("0");
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) lcd.print("0");
  lcd.print(second);
}

// Hàm hiển thị thời gian trên LCD
void time_display() {
  tmElements_t tm;
  RTC.read(tm); // Đọc thời gian từ DS1307

  lcd.setCursor(0, 0);
  lcd.print(tm.Day);

  lcd.setCursor(2, 0);
  lcd.print("/");

  lcd.setCursor(3, 0);
  lcd.print(tm.Month);

  lcd.setCursor(5, 0);
  lcd.print("/");

  lcd.setCursor(9, 0);
  lcd.print(tmYearToCalendar(tm.Year));

  lcd.setCursor(0, 1);
  lcd.print(tm.Hour < 10 ? "0" : "");
  lcd.print(tm.Hour);
  lcd.print(":");
  lcd.print(tm.Minute < 10 ? "0" : "");
  lcd.print(tm.Minute);
  lcd.print(":");
  lcd.print(tm.Second < 10 ? "0" : "");
  lcd.print(tm.Second);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  lcd.setCursor(13, 0);
  lcd.print(h,0); // Hiển thị ba chữ cái đầu tiên của ngày trong tuần
  
  lcd.setCursor(10, 1);
  lcd.print(t,1); // Hiển thị nhiệt độ
  lcd.setCursor(14, 1);
  lcd.print((char)223);
  lcd.print("C");
  delay(1000);
}


// Hàm khởi tạo các nút
void Init_button() {
  pinMode(Toggle_Up, INPUT_PULLUP);
  pinMode(Set_date, INPUT_PULLUP);
  pinMode(Set_time, INPUT_PULLUP);
}

// Hàm khởi tạo LCD
void Init_lcd() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("DONG HO!");
  lcd.setCursor(5, 1);
  lcd.print("HELLO");
  delay(1000);
  lcd.clear();
}

void setup() {
  Serial.begin(9600);

  bool parse = false;
  bool config = false;
  tmElements_t tm; // Khai báo biến tm tại đây
  dht.begin();

  Init_lcd();
  Init_button();

  attachInterrupt(0, Toggle_Up_Value, FALLING); // Gắn hàm xử lý khi nút Up được nhấn
  attachInterrupt(1, Toggle_Down_Value, FALLING); // Gắn hàm xử lý khi nút Down được nhấn
}

void loop() {
  if (!digitalRead(Set_time)) { // Khi nút Set time được nhấn
    while (!digitalRead(Set_time));
    check_inter_0 = false;
    set_time_button();
    lcd.clear();
  }


  if (!digitalRead(Set_date)) { // Khi nút Set date được nhấn
    while (!digitalRead(Set_date));
    check_inter_0 = false;
    check_inter_1 = false;
    set_date_button();
    lcd.clear();
  }

  time_display(); // Hiển thị thời gian
}
