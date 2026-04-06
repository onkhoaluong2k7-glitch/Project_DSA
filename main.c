#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//====================================================================
//                  KHAI BAO CAC BIEN, CAC KIEU DU LIEU
//====================================================================
#define MAX_NAME 50
#define MAX_NOTE 100
// Trạng thái đơn hàng
typedef enum{
    DANG_PHUC_VU,
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
    char thoiGianCapNhat[20];
    char ghiChu[MAX_NOTE];
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
    TrangThai trangThai;
    struct Order* next;
} Order;

//================================================================================
//                          CAC HAM XU LY
//================================================================================
// Tạo món ăn mới
Dish* TaoMonAnMoi(const char* ma, const char* ten, int soluong){
    Dish* monMoi = (Dish*) malloc(sizeof(Dish));
    strcpy(monMoi->maMon, ma);
    strcpy(monMoi->tenMon, ten);
    monMoi->soLuongDat = soluong;

    monMoi->soLuongTra = 0;
    strcpy(monMoi->ghiChu, "");
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
    donMoi->trangThai = DANG_PHUC_VU;
    donMoi->next = NULL;
    donMoi->danhSachMon = NULL;

    return donMoi;
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
// ========================================================================= 
//                  CHUONG TRINH CHINH
// =========================================================================
int main(){ 
    printf("====HE THONG QUAN LY NHA HANG CUA ON KHOA LUONG====\n");
    Order* banso5 = TaoOrderMoi(5, "Luong");
    strcpy(banso5->thoiGian, "20:07 8/3/2028");

    Dish* mon1 = TaoMonAnMoi("M01", "Tra Sua", 2);
    Dish* mon2 = TaoMonAnMoi("M02", "Khoai Mon Le Pho", 4);
    ThemMonAnVaoOrder(banso5, mon1);
    ThemMonAnVaoOrder(banso5, mon2);
    
    banso5->trangThai = DA_THANH_TOAN;
    LuuHoaDonRaFile(banso5, "LichSuBanHang.txt");

    return 0;
}