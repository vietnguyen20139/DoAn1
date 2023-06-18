#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LinkedList.h>

#define SS_PIN 10  // Chân SDA
#define RST_PIN 9

Servo myservo;

int yellow_led = 5;
int red_led = 7;
int mic = 8;

MFRC522 rfid(SS_PIN, RST_PIN);
// MFRC522::MIFARE_Key key;

//khai bao trang thai cua cong la cong vao
//mac dinh cong vao
bool stateCong = 1;
unsigned long getTime;
bool cothe = false;

bool in1lan;

String strID;
int maxXe = 3;
int soXe = 0;
bool isMax = false;
int len;  //do dai cua chuoi tu esp32 gui qua

//tao 2 cau truc de chua the dang ky va the khach
class dangky {
public:
  String ID;
  bool stateID = 0;
};

class khach {
public:
  String ID;
  unsigned long vaoCongTime;
};

//tao 2 danh sach de chua the
LinkedList<dangky *> listDKy = LinkedList<dangky *>();
LinkedList<khach *> listKhach = LinkedList<khach *>();

khach *checkKhach = new khach();
dangky *checkDK = new dangky();

void setup() {
  in1lan = true;

  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();      // Initiate  SPI bus
  rfid.PCD_Init();  // Initiate MFRC522

  pinMode(5, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  myservo.attach(6);
  myservo.write(0);

  //tao 1 the da dang ky
  checkDK->ID = "E0:D4:58:19";

  //them the vao danh sach dang ky
  listDKy.add(checkDK);

  // Serial.println("Chế độ cổng hiện tại là cổng vào!");
  Serial.println(" sx" + String(soXe)+"$");
  Serial.println(" mx" + String(maxXe)+"$");
  // inMax();
}


void loop() {

  if (Serial.available() > 0) {
    dieuKhien();
  }

  if (!kiemThe())
    return;

  xuLyStr();
  // Serial.println(" mt" + strID + "$");

  // strop reading
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  //kiem tra strID co trong danh sach dang ky hay khong
  if (cothe) {
    //co the thi kiem tra the co nam trong 2 danh sach khong
    for (int i = 0; i < listDKy.size(); i++) {
      //gan tung phan tu trong listDK cho checkDK de kiem tra tung phan tu
      checkDK = listDKy.get(i);
      if (checkDK->ID == strID) {
        // Serial.println("Thẻ có trong danh sách đăng ký!");
        //neu stateID = 1 tuc la the da vao cong, 0 la chua vao
        if (checkDK->stateID == stateCong) {
          // Serial.println(" thongbaoCảnh báo sai cổng! $");
          canhbao();
          return;
        } else {
          //mo cong cho xe vao
          if (!stateCong) {
            access("dangky");
            soXe--;
            isMax = false;
            checkDK->stateID = stateCong;
          } else if ((!isMax) && stateCong) {
            checkDK->stateID = stateCong;
            access("dangky");
            // Serial.println(isMax);
            soXe++;
          } else {
            // Serial.println(" thongbaoCảnh báo hết chỗ để xe! $");
            Serial.println(" tb1$");
            delay(500);

            loa();
          }
        }
        cothe = 0;  //set = 0 de khong can kiem tra tiep the trong danh sach khach
        // Serial.println("thoat vong lap danh sach dang ky");
        break;
      }
    }
    // Serial.println("da thoat vong lap danh sach dang ky");
  }

  if (cothe) {
    //kiem tra strID co trong danh sach khach hay khong
    for (int i = 0; i < listKhach.size(); i++) {
      //gan tung phan tu trong listDK cho checkDK de kiem tra tung phan tu
      checkKhach = listKhach.get(i);
      //kiem tra the khach da co trong danh sach chua
      //neu co tuc la xe khach da vao, chi co the quet the khach o loi ra day la loi vao
      //neu chua co thi them the khach vao danh sach khach, mo cong
      if (checkKhach->ID == strID) {
        // Serial.println("Thẻ có trong danh sách đăng ký!");
        if (stateCong) {
          // Serial.println(" thongbaoCảnh báo sai cổng! $");
          canhbao();
          return;
        } else {
          tinhTien();
          // Serial.println("Xóa thẻ " + strID + " khỏi danh sách khách!");
          listKhach.remove(i);
          access("khach");
          isMax = false;
          soXe--;
        }
        cothe = 0;  //set = 0 de khong can kiem tra tiep the trong danh sach khach
        // Serial.println("thoat vong lap danh sach khach");
        break;
      }
    }
  }

  if (cothe) {
    // Serial.println("da thoat vong lap danh sach khach, the khong co trong danh sach khach");
    //sau khi thoat vong lap tuc la the khong co trong danh sach khach
    if (stateCong) {
      khach *inKhach = new khach();
      if (!isMax) {
        inKhach->ID = strID;
        inKhach->vaoCongTime = millis();
        // Serial.println("thêm thẻ " + strID + " vào danh sách khách");
        listKhach.add(inKhach);
        access("khach");
        // Serial.println(isMax);
        soXe++;
      } else {
        // Serial.println(" thongbaoCảnh báo hết chỗ để xe! $");
        Serial.println(" tb1$");
        delay(500);

        loa();
        return;
      }
    } else
      canhbao();
    // Serial.print("so luong trong danh sach khach ");
    // Serial.println(listKhach.size());
  }
  delay(200);
  Serial.println(" sx" + String(soXe) + "$");
  delay(1);

  if (soXe == maxXe) {
    // Serial.println(" thongbaoHết chỗ để xe! $");
    // Serial.println(" tb20$");
    // delay(1);

    isMax = true;
    // Serial.println(isMax);
    // return;
  }
}

void inMax() {
  String guiMax = "^" + String(maxXe) + "^";
  Serial.println(guiMax);
}
void xuLyStr() {
  strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX) + (i != 3 ? ":" : "");
  }
  strID.toUpperCase();
  Serial.println(" mt" + strID + "$");
  delay(1);


  // Serial.println(strID);
}

bool kiemThe() {
  // if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
  if (in1lan) {
    // Serial.print(" thongbaoĐang đợi nhận thẻ $");
    Serial.println(" tb3$");
    delay(1);

    // Serial.println(stateCong ? "vào" : "ra");
  }
  in1lan = false;
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Serial.println("Đã nhận thẻ!");
    in1lan = true;
    cothe = true;
    return true;
  }
  return false;
}

void dieuKhien() {
  String check = Serial.readStringUntil('\n');
  // Serial.println(check);
  len = check.length();
  // if ((len == 2) && (check.indexOf('1') == 0)) {
  if ((len == 1) && (check.indexOf('1') == 0)) {

    stateCong = 1;
    // Serial.println("Đặt chế độ cổng là cổng vào");
    // Serial.println("Đang đợi nhận thẻ!");

    loa();
    loa();
    loa();
    in1lan = true;
    // break;
  }

  // if ((len == 2) && (check.indexOf('0') == 0)) {
  if ((len == 1) && (check.indexOf('0') == 0)) {

    stateCong = 0;
    // Serial.println("Đặt chế độ cổng là cổng ra");
    // Serial.println("Đang đợi nhận thẻ!");

    loa();
    loa();
    loa();
    loa();
    in1lan = true;
    // break;
  }

  // if ((len == 4) && (check.indexOf("add") == 0)) {
  if ((len == 3) && (check.indexOf("add") == 0)) {

    // Serial.println(" thongbaoMời quét thẻ trong 3s! $");
    Serial.println(" tb4$");
    delay(1);

    getTime = millis();
    while ((unsigned long)(millis() - getTime) < 3000) {
      if (kiemThe())
        break;
    }

    if (cothe) {
      bool inKhach = false, inDky = false;

      xuLyStr();
      // Serial.println(" mt" + strID + "$");

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();

      for (int i = 0; i < listKhach.size(); i++) {
        checkKhach = listKhach.get(i);
        if (checkKhach->ID == strID) {
          // Serial.println(" thongbaoThẻ " + strID + " đã có trong danh sách khách! $");
          Serial.println(" tb5$");
          delay(1);

          inKhach = true;
          break;
        }
      }

      for (int i = 0; i < listDKy.size(); i++) {
        checkDK = listDKy.get(i);
        if (checkDK->ID == strID) {
          // Serial.println(" thongbaoThẻ " + strID + " đã có trong danh sách đăng ký! $");
          Serial.println(" tb6$");
          delay(1);

          inDky = true;
          break;
        }
      }

      if (inKhach || inDky) {
        // Serial.println(" thongbao2Trạng thái thẻ không phù hợp để thêm vào danh sách đăng ký $");
        Serial.println(" tb21$");
        delay(1);

      } else {
        dangky *addDK = new dangky();
        addDK->ID = strID;
        addDK->stateID = false;
        listDKy.add(addDK);
        // Serial.println(" thongbaothêm thẻ " + strID + " vào danh sách đăng ký! $");
        Serial.println(" tb7$");
        delay(1);
      }

    } else {
      // Serial.println(" thongbaoHết thời gian! $");
      Serial.println(" tb8$");
      delay(1);
    }
    cothe = false;
    loa();
    loa();
    loa();
    in1lan = true;
    // Serial.println(" thongbao ");
    // Serial.println(" thongbao2 ");

    // break;
  }

  // if ((len == 7) && (check.indexOf("remove") == 0)) {
  if ((len == 6) && (check.indexOf("remove") == 0)) {

    Serial.println(" tb9$");
    // Serial.println(" thongbaoMời quét thẻ trong 3s! $");
    delay(1);


    getTime = millis();
    while ((unsigned long)(millis() - getTime) < 3000) {
      if (kiemThe())
        break;
    }

    if (cothe) {
      xuLyStr();
      // Serial.println(" mt" + strID + "$");

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();

      bool inDky1 = false;
      for (int i = 0; i < listDKy.size(); i++) {
        checkDK = listDKy.get(i);
        if (checkDK->ID == strID) {
          // Serial.println(" thongbaothẻ " + strID + " có trong danh sách đăng ký! $");
          Serial.println(" tb6$");
          delay(1);
          inDky1 = true;
          if (!(checkDK->stateID)) {
            listDKy.remove(i);
            Serial.println(" tb10$");
            delay(1);

            // Serial.println(" thongbaoxóa thẻ " + strID + " khỏi danh sách đăng ký! $");

          } else {
            Serial.println(" tb22$");
            delay(1);

            // Serial.println("Trạng thái thẻ không phù hợp để xóa khỏi danh sách đăng ký!");
          }
          break;
        }
      }
      if (!inDky1)
        Serial.println(" tb11$");
      delay(1);

      // Serial.println("thẻ " + strID + " không nằm trong danh sách đăng ký, không thể xóa!");

    } else {
      // Serial.println(" thongbaoHết thời gian! $");
      Serial.println(" tb8$");
      delay(1);
    }
    cothe = false;
    loa();
    loa();
    loa();
    in1lan = true;
    // break;
    // Serial.println(" thongbao ");
    //   Serial.println(" thongbao2 ");
  }

  if (check.indexOf("max") == 0) {
    // Serial.println("mời cập nhật sức chứa của bãi xe!");
    String str = check.substring(3, len);
    if (kiemtra(str)) {
      maxXe = str.toInt();
      // Serial.print("Sức chứa hiện tại: ");
      // Serial.println(maxXe);
      isMax = false;
      // inMax();
    } else {
      // Serial.println(" thongbaoNhập sai! $");
      Serial.println(" tb12$");
      delay(1);
    }

    loa();
    loa();
    loa();
    in1lan = true;

    // break;
  }
  delay(1500);
}

bool kiemtra(String str) {
  for (byte i = 0; i < str.length() - 1; i++) {
    if (!isDigit(str.charAt(i))) {
      return false;
    }
  }
  return true;
}

void access(String check) {
  if (check == "khach") {
    // Serial.println(" thongbao**MỞ CỔNG XE KHÁCH** $");
    Serial.println(" tb13$");
    delay(1);
  }
  if (check == "dangky") {
    // Serial.println(" thongbao**MỞ CỔNG XE ĐĂNG KÝ** $");
    Serial.println(" tb14$");
    delay(1);
  }
  digitalWrite(5, HIGH);
  digitalWrite(7, LOW);
  loa();
  //myservo.write(90); //motor moves 90 degree
  myservo.write(90);   // Open servo
  delay(3000);        // Wait for 3 seconds
  myservo.write(0);  // Close servo
  digitalWrite(5, LOW);
}

void canhbao() {
  // Serial.println(" thongbao**CẢNH BÁO SAI CỔNG** $");
  Serial.println(" tb15$");
  delay(1);

  digitalWrite(5, LOW);
  digitalWrite(7, HIGH);

  loa();
  loa();

  //myservo.write(90); //motor moves 90 degree
  delay(1000);  // Wait for 3 seconds
  digitalWrite(7, LOW);
}

void tinhTien() {
  //cu 2s la 5k
  int soLan2s = (unsigned long)(millis() - checkKhach->vaoCongTime) / 2000 + 1;
  int tien = soLan2s * 5;
  // Serial.println(" thongbaoSố tiền phải trả: $");
  Serial.println(" tt" + String(tien) + "$");
  delay(1);

  // Serial.println("k");
}

void loa() {
  digitalWrite(8, HIGH);
  delay(100);
  digitalWrite(8, LOW);
  delay(100);
}
