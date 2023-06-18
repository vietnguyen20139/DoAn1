import sys
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtSerialPort import QSerialPort
from BangDieuKhien import Ui_MainWindow

import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
import time

cred = credentials.Certificate('doan-1-1c412-firebase-adminsdk-hhk8a-0e05fe99a3.json')
# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(cred, {
    'databaseURL': "https://doan-1-1c412-default-rtdb.asia-southeast1.firebasedatabase.app/"
})

ref = db.reference("/")
ref.update({"max": "2"})

class Main(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self):
        super(Main, self).__init__()
        self.setupUi(self)
        self.port = QSerialPort()
        self.data = QtCore.QByteArray()
        self.listThongBao = ["tb1$", "tb20$", "tb3$","tb4$", "tb5$", "tb6$","tb21$", "tb7$", "tb8$","tb9$", "tb10$", "tb22$","tb11$","tb12$", "tb13$", "tb14$", "tb15$" ]
        self.setWindowTitle('BangDieuKhien')
        
        self.portOpen()

        self.vaoBtn.clicked.connect(lambda: self.sendFromPort("1"))
        self.raBtn.clicked.connect(lambda: self.sendFromPort("0"))
        self.addBtn.clicked.connect(lambda: self.sendFromPort("add"))
        self.removeBtn.clicked.connect(lambda: self.sendFromPort("remove"))
        self.capNhatBtn.clicked.connect(lambda: self.sendFromPort("max"))


        self.port.readyRead.connect(self.readFromPort)

    def portOpen(self):
        self.port.setBaudRate(115200)
        self.port.setPortName('COM7')
        self.port.open(QtCore.QIODevice.ReadWrite)
  
        
    def readFromPort(self):
        # if !(self.port.waitForReadyRead(100)):
        #     self.data.remove(0, len(self.data))
        #     print(self.data)
        if self.port.waitForReadyRead(100):
            self.data += self.port.readAll()
        
        print("-------------------")
        if len(self.data) > 0:
            inra = self.data.data().decode('utf-8', errors='ignore')
            print(inra)

            if ("mt" in inra) and (inra.find("$") == 14):
                print(inra)
                x = inra[3:14]
                self.maThe.setText(x)
                # print("x la " + x)
                # print(len(inra))
                self.data.clear()

            if ("sx" in inra) and (inra.find("$") > 0):
                print(inra)
                y = inra.strip()[2:3]
                self.soXe.setText(y)
                ref.update({"soxe": y})
                print("y la " + y)
                # print(len(inra))
                self.data.clear()

            if ("tt" in inra) and (inra.find("$") > 0):
                print(inra)
                z = inra.strip()[2:inra.find("$")-1]
                # self.ThongBao.setText("Số tiền phải trả:")
                self.ThongBao2.setText(z + " nghìn đồng")                
                print("z la " + z)
                # print(len(inra))
                self.data.clear()
            

            for i in self.listThongBao:
                if ((i in inra) and (inra.find('$')>0)):
                    # print("ok" + i)
                    self.switch(i)
                    self.data.clear()
            print("..................")

    def sendFromPort(self, _str):
        if (_str == "max") and (self.nhapMax.text().isdigit()):
            _str = _str + self.nhapMax.text()
            o = self.nhapMax.text()
            ref.update({"max": o})
            print(_str)
            print(len(_str))
            self.maxXe.setText(o)
            self.nhapMax.setText("")
        else:
            self.ThongBao.setText("Nhập sai")
            self.ThongBao2.setText(" ")
            print("nhap sai")
            # time.sleep(1)
            self.nhapMax.setText("")

        if _str == "1":
            self.TrangThaiCong.setText("CỔNG VÀO")
        if _str == "0":
            self.TrangThaiCong.setText("CỔNG RA")
        data = _str.encode("utf-8")
        self.port.write(data)
    
    def switch(self, check):
        if check == "tb1$":
            self.ThongBao.setText("hết chỗ để xe")
            self.ThongBao2.setText(" ")

        elif check == "tb20$":
            self.ThongBao2.setText("Hết chỗ để xe")
            self.ThongBao.setText(" ")

        elif check == "tb3$":
            self.ThongBao.setText("Đang đợi nhận thẻ")
            self.ThongBao2.setText(" ")

        elif check == "tb4$":
            self.ThongBao.setText("Mời quét thẻ trong 3s")
            self.ThongBao2.setText(" ")
            
        elif check == "tb5$":
            self.ThongBao.setText("Thẻ khách đã quét vào cổng")
            self.ThongBao2.setText(" ")
            # time.sleep(1)
            
        elif check == "tb6$":
            self.ThongBao.setText("Thẻ có trong danh sách đăng ký")
            self.ThongBao2.setText(" ")
            # time.sleep(1)

        elif check == "tb21$":
            self.ThongBao2.setText("Thẻ không phù hợp để đăng ký")
            
        elif check == "tb7$":
            self.ThongBao.setText("Đăng ký thành công")
            self.ThongBao2.setText(" ")
            
        elif check == "tb8$":
            self.ThongBao.setText("Hết thời gian")
            self.ThongBao2.setText(" ")
                    
        elif check == "tb9$":
            self.ThongBao.setText("Mời quét thẻ trong 3s")
            self.ThongBao2.setText(" ")
                    
        elif check == "tb10$":
            self.ThongBao2.setText("xóa thẻ thành công")
            
        elif check == "tb22$":
            self.ThongBao2.setText("Thẻ đã quét vào cổng, không thể xóa")
            
        elif check == "tb11$":
            self.ThongBao.setText("thẻ không nằm trong danh sách đăng ký")
            self.ThongBao2.setText("Không thể xóa")
            
        elif check == "tb12$":
            self.ThongBao.setText("Nhập sai")
            self.ThongBao2.setText(" ")
            
        elif check == "tb13$":
            self.ThongBao.setText("MỞ CỔNG XE KHÁCH")
            self.ThongBao2.setText(" ")
            
        elif check == "tb14$":
            self.ThongBao.setText("MỞ CỔNG XE ĐĂNG KÝ")
            self.ThongBao2.setText(" ")
            
        elif check == "tb15$":
            self.ThongBao2.setText("CẢNH BÁO SAI CỔNG")
            self.ThongBao.setText(" ")
            
        
                  

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = Main()
    window.show()
    app.exec()