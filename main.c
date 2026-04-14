#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sqlite3.h"
//====================================================================
//                  KHAI BAO CAC BIEN, CAC KIEU DU LIEU
//====================================================================
#define MAX_NAME 50
#define MAX_NOTE 100
// Trạng thái đơn hàng
typedef enum{
    MOI_TAO,
    DANG_LAM,
    DA_XONG,
    DA_TRA_MON,
    DA_THANH_TOAN,
    DON_HUY  
} TrangThai;

// Cấu trúc món ăn
typedef struct Dish{
    char maMon[MAX_NAME];
    char tenMon[MAX_NOTE];
    char thoigian[20];
    int soLuongDat;
    int soLuongTra;
    //char thoiGianCapNhat[20];
    char ghiChu[MAX_NOTE];
    TrangThai trangThai;
    int long gia;

    struct Dish* next;
} Dish;

//Cấu trúc đơn hàng
typedef struct Order{
    char thoiGian[20];
    char tenNhanVien[MAX_NAME];
    int maBan;
    Dish* danhSachMon;
    int tongSoMon;
    int tongSoDiaDat;
    int tongSoMonTra;
    int tongSoDiaTra;
    char thoiGianCapNhat[20];
    long tien;
    TrangThai trangThai;
    struct Order* next;
} Order;

//================================================================================
//                          CAC HAM XU LY
//================================================================================
//Ham lay thoi gian hien tai
void LayThoiGianHienTai(char* buffer){
    time_t thoiGian = time(NULL);
    struct tm *thoiGianChuan = localtime(&thoiGian);
    strftime(buffer, 20, "%H:%M %d/%m/%Y", thoiGianChuan);
}
// Tạo món ăn mới
Dish* TaoMonAnMoi(const char* ma, const char* ten, int soluong){
    Dish* monMoi = (Dish*) malloc(sizeof(Dish));
    strcpy(monMoi->maMon, ma);
    strcpy(monMoi->tenMon, ten);
    monMoi->soLuongDat = soluong;

    monMoi->soLuongTra = 0;
    strcpy(monMoi->ghiChu, "");
    monMoi->trangThai = MOI_TAO;
    //LayThoiGianHienTai(monMoi->thoiGianCapNhat);
    monMoi->next = NULL;

    return monMoi;
}
// Tạo Order mới
Order* TaoOrderMoi(int soban, const char* nhanVien){
    Order* donMoi = (Order*) malloc(sizeof(Order));

    donMoi->maBan = soban;
    strcpy(donMoi->tenNhanVien, nhanVien);

    donMoi->tongSoMon = 0;
    donMoi->tongSoDiaDat = 0;
    donMoi->tongSoMonTra = 0;
    donMoi->tongSoDiaTra = 0;
    donMoi->trangThai = DANG_LAM;
    donMoi->next = NULL;
    donMoi->danhSachMon = NULL;

    return donMoi;
}

//Hàm thay đổi trạng thái 
void ThayDoiTrangThaiMonAn(Dish* temp,  TrangThai tt){
    if ( temp != NULL) {
        temp->trangThai = tt;
    }
}
// Hàm thêm món ăn mới vào order
void ThemMonAnVaoOrder(Order* donHang, Dish* monMoi){
    if (donHang == NULL || monMoi == NULL)  return;

    if (donHang->danhSachMon == NULL){
        donHang->danhSachMon = monMoi; 
    } else {
        Dish* temp = donHang->danhSachMon;
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = monMoi;
    }

    donHang->tongSoMon++;
    donHang->tongSoDiaDat += monMoi->soLuongDat;
}
// Hàm xóa món ăn khỏi order
void XoaMonAnTrongOrder(Order* donHang, const char* ma){
    if (donHang == NULL || donHang->danhSachMon == NULL) return;

    Dish* temp = donHang->danhSachMon;
    if (strcmp(temp->maMon, ma) == 0){
        donHang->danhSachMon = temp->next;
        donHang->tongSoMon--;
        donHang->tongSoDiaDat -= temp->soLuongDat;
        free(temp);
        return;
    }
    Dish* prev = NULL;
    while(temp != NULL){
        if (strcmp(temp->maMon, ma) == 0){
            prev->next = temp->next;
            donHang->tongSoMon--;
            donHang->tongSoDiaDat -= temp->soLuongDat;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Khong tim thay mon an de xoa!!");
}
// hàm hủy đơn hàng
void HuyDonHang(Order* donHang){
    if (donHang == NULL) return;
    if (donHang->tongSoDiaTra > 0) {
        printf("Khong the huy don! Do bep da tra mon!\n");
        return;
    }

    donHang->trangThai = DON_HUY;
    LayThoiGianHienTai(donHang->thoiGianCapNhat);

}

// Hàm này để hủy món do Khách yêu cầu
void KhachHuyMon(Order* ban, Dish* mon){
    if (mon->trangThai == MOI_TAO){
        mon->trangThai = DON_HUY;
        strcpy(mon->ghiChu, "Khach HUY do cho qua lau!!!");
        LayThoiGianHienTai(mon->trangThai);
        mon->soLuongTra = 0;
        return;
    }

    printf("Khong the huy duoc! Do bep da chuan bi!");
    return;
}
// Hàm tính tiền thanh toán
void TinhTienHoaDon(Order* donHang){
    if (donHang == NULL || donHang->danhSachMon == NULL) return;
    long total = 0;
    Dish* temp = donHang->danhSachMon;
    while (temp != NULL){
        if (temp->trangThai == DA_TRA_MON){
            total += (temp->soLuongTra)*(temp->gia);
        }
        temp = temp->next;
    }
    donHang->tien = total;
    return;
}
// In hóa đơn ra file txt
void LuuHoaDonRaFile(Order* donHang, const char* tenfile){
    if (donHang == NULL || donHang->danhSachMon == NULL) return;
    
    if (donHang->trangThai != DA_THANH_TOAN) {
        printf("Don hang %d chua thanh toan. Vui long thanh toan!!", donHang->maBan);
        return;
    }

    FILE* file = fopen(tenfile, "a");
    if (file == NULL){
        printf("Loi: khong tim thay file %s!\n", tenfile);
        return;
    }

    fprintf(file,"========================================================\n");
    fprintf(file,"       HOA DON BAN SO: %d\n.", donHang->maBan );
    fprintf(file,"       Nhan Vien: %s\n.",donHang->tenNhanVien );
    fprintf(file,"       Thoi gian tao: %s\n ", donHang->thoiGian);
    fprintf(file,"--------------------------------------------------------\n");
    Dish* temp = donHang->danhSachMon;
    int stt = 1;
    while (temp != NULL){
        fprintf(file,"%d. %-15s | SL: %d | Trang thai: Da Tra\n", stt, temp->tenMon, temp->soLuongDat);
        temp = temp->next;
        stt++;
    }
    fprintf(file,"-------------------------------------------------------\n");
    fprintf(file, "Tong so mon: %d | Tong so dia: %d\n", donHang->tongSoMon, donHang->tongSoDiaDat);
    fprintf(file,"##### DA THANH TOAN ####\n");
    fprintf(file,"========================================================\n");
    fclose(file);
    printf("Da luu hoa don vao file.txt thanh cong!!\n");
}
//Hàm để in hóa đơn ra ngoài màn hình
void InHoaDonRaNgoaiManHinh(Order* donHang){
    if (donHang == NULL || donHang->danhSachMon == NULL) return;

    printf("========================================================\n");
    printf("       HOA DON BAN SO: %d\n.", donHang->maBan );
    printf("       Nhan Vien: %s\n.",donHang->tenNhanVien );
    printf("       Thoi gian tao: %s\n ", donHang->thoiGian);
    printf("--------------------------------------------------------\n");
    Dish* temp = donHang->danhSachMon;
    int stt = 1;
    while(temp != NULL){
        printf("%d. %-15s | SL: %d | Trang thai: Da Tra\n", stt, temp->tenMon, temp->soLuongDat);
        temp = temp->next;
        stt++;
    }
    printf("-------------------------------------------------------\n");
    printf("Tong so mon: %d | Tong so dia: %d\n", donHang->tongSoMon, donHang->tongSoDiaDat);
    printf("##### DA THANH TOAN ####\n");
    printf("========================================================\n");
};

// ========================================================================= 
//                  CHUONG TRINH CHINH
// =========================================================================
int main(){ 
    printf("====HE THONG QUAN LY NHA HANG CUA ON KHOA LUONG====\n");

    Order* banso = TaoOrderMoi(1, "Luong");
    
    Dish* mon1 = TaoMonAnMoi("M01", "Tra Sua", 2);
    Dish* mon2 = TaoMonAnMoi("M02", "Khoai Mon Le Pho", 4);
    while(1){
        printf("1. Tao danh sach cac mon an.\n");
        printf("2. Them mon an.\n");
        printf("3. Xoa mon.\n");
        printf("4. Xem hoa don. \n");
        printf("Vui long chon: ");
        int opt;
        scanf("%d", &opt);
        switch (opt){
            case 1:
                printf("Da tao xong!\n");
                break;
            case 2:
                ThemMonAnVaoOrder(banso, mon1);
                ThemMonAnVaoOrder(banso, mon2);
                break;
            case 4:
                LuuHoaDonRaFile(banso,"LichSuBanHang.txt");
                break;
            default:
                printf("Lua chon khong hop le!\nVui long nhap lai!\n");
                break;
        }
        break;
    }

    return 0;
}