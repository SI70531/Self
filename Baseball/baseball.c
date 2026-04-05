#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// ==========================================
//   1. 巨集與設定 (絕對置頂)
// ==========================================
#define MAX_LV 10
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// 現代 UI 元素
#define UI_LINE_H   "──────────────────────────────────────────────"
#define UI_LINE_DBL "══════════════════════════════════════════════"
#define ICON_STAR   "★"
#define ICON_ARROW  "»"
#define ICON_BULLET "●"
#define ICON_CHECK  "✓"
#define ICON_CROSS  "✕"
#define ICON_GEAR   "⚙"

// 相容性定義
#define BOX_DBL_H "═"

// 顏色代碼 (ANSI Escape Codes)
#define C_RESET   "\x1b[0m"
#define C_BOLD    "\x1b[1m"
#define C_DIM     "\x1b[2m"
#define C_RED     "\x1b[31m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m" 
#define C_BLUE    "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN    "\x1b[36m"
#define C_WHITE   "\x1b[37m"
#define C_GRAY    "\x1b[90m"

// 高級配色
#define C_GOLD    "\x1b[38;5;220m"
#define C_ROYAL   "\x1b[38;5;39m" 
#define C_CRIT    "\x1b[38;5;198m" 
#define C_NEON_CYAN "\x1b[1;96m"   

// 卡片等級背景色 (Badges)
#define BG_BRONZE "\x1b[43;30m"      
#define BG_SILVER "\x1b[47;30m"      
#define BG_GOLD   "\x1b[103;30m"     
#define BG_DIAMOND "\x1b[106;30m"    
#define BG_MASTER  "\x1b[41;1;37m"   

#define BG_CRIT_LBL    "\x1b[45;1;37m"   
#define BG_SUCCESS_LBL "\x1b[42;1;37m"   
#define BG_FAIL_LBL    "\x1b[41;1;37m"   

// 卡片等級文字色 (List)
#define COL_1  "\x1b[33m"      
#define COL_23 "\x1b[37m"      
#define COL_46 "\x1b[1;33m"    
#define COL_79 "\x1b[1;36m"    
#define COL_10 "\x1b[1;31m" 

#define COLOR_BRONZE COL_1
#define COLOR_SILVER COL_23
#define COLOR_GOLD   COL_46
#define COLOR_BLUE   COL_79
#define COLOR_RED_T  COL_10

// ==========================================
//   2. 唯讀資料與狀態結構 (Global Data & Structs)
// ==========================================

// 機率表
const double PROB_TABLE[10][11] = {
    {0},
    {0, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
    {0, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
    {0, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
    {0, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
    {0, 11.80, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0},
    {0, 4.44, 11.80, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0}, 
    {0, 1.25, 4.44, 11.80, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0}, 
    {0, 0.24, 1.25, 4.44, 11.80, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0}, 
    {0, 0.02, 0.24, 1.25, 4.44, 11.80, 25.12, 44.53, 67.68, 90.00, 100.0} 
};

// 狀態結構體
typedef struct {
    int language_mode;
    bool is_event_active;
    bool force_low_lv_use_one;
    int strategy_mode;
    int operation_mode;
    int inventory[MAX_LV + 1];
    int lack_count;
    int main_card_lv;
    bool force_exit_flag;
    bool interrupt_flag;
} SimState;

// 堆疊結構 (Stack Frame)
typedef struct {
    int curr;
    int target;
    int depth;
} SynthFrame;

// ==========================================
//   3. 函式原型 (Prototypes)
// ==========================================
void init_console();
void enable_raw_mode();
void disable_raw_mode();
int check_keyboard_hit();
char read_char_no_block();
void clear_buffer();
void pause_key();
int get_str_width(const char* s);
void ui_header(const char* title);
void ui_line();
void ui_item(const char* text);
void ui_title_text(const char* text);
void print_card_fmt(int lv, char* buffer);
void print_card_text(int lv, char* buffer);
void print_tree(int depth, bool is_end); 
void play_bar(const char* label, const char* color);
bool wait_action(SimState* st);
const char* get_strat_name(int s);
const char* get_strat_short_name(int s);
const char* get_op_mode_name(int m);
int get_mat_lv(SimState* st, int target);
int get_req_mat(SimState* st, int target);
void show_inventory(SimState* st);
void setup_settings(SimState* st);
void ask_low_level_rule(SimState* st);
void setup_stock(SimState* st);
void buy_one(SimState* st, int depth);
void prep_mat(SimState* st, int target, int depth);
void do_synth(SimState* st, int curr, int target, int depth);
bool perform_bg_check(SimState* st, int *curr, int req, int depth);
void run_sim(SimState* st);
long long calc_100_path(SimState* st, int lv);
void draw_path(SimState* st, int lv, int depth);
double calc_avg_cost(SimState* st, int lv, int strat, double* memo);
void run_path(SimState* st);
void run_avg(SimState* st);
double get_rnd_pct();
double get_rnd_01();
int get_auto_best_stock(SimState* st, int main_lv, int limit_lv);
void clear_screen();
int get_input_int(int default_val);

// ==========================================
//   4. 跨平台設定
// ==========================================
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #define SLEEP_MS(x) Sleep(x)
    void init_console() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= 0x0004; 
        SetConsoleMode(hOut, dwMode);
        SetConsoleOutputCP(65001); 
    }
    void enable_raw_mode() {}
    void disable_raw_mode() {}
    int check_keyboard_hit() { return _kbhit(); }
    char read_char_no_block() { return _getch(); }
    void clear_screen() { system("cls"); }
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #include <sys/select.h>
    #define SLEEP_MS(x) usleep((x)*1000)
    struct termios orig_termios;
    void init_console() { tcgetattr(STDIN_FILENO, &orig_termios); }
    void enable_raw_mode() {
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }
    void disable_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }
    int check_keyboard_hit() {
        struct timeval tv = {0L, 0L};
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    }
    char read_char_no_block() {
        char c; if (read(STDIN_FILENO, &c, 1) < 0) return 0; return c;
    }
    void clear_screen() { printf("\033[H\033[J"); } 
#endif

// ==========================================
//   5. 核心工具
// ==========================================
void clear_buffer() { int c; while ((c = getchar()) != '\n' && c != EOF); }
double get_rnd_pct() { return (double)rand() / (double)RAND_MAX * 100.0; }
double get_rnd_01() { return (double)rand() / (double)RAND_MAX; }

int get_input_int(int default_val) {
    char buf[100];
    while (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf, "\r\n")] = '\0';
        if (strlen(buf) == 0) return default_val; 
        
        char *endptr;
        long val = strtol(buf, &endptr, 10);
        if (endptr != buf && *endptr == '\0') {
            return (int)val;
        }
        printf("   " C_RED ">> 輸入無效啦，請填寫數字喔: " C_RESET);
    }
    return default_val;
}

int get_str_width(const char* s) {
    int len = 0, i = 0;
    while (s[i]) {
        if (s[i] == '\x1b') { while (s[i] && s[i] != 'm') i++; if (s[i]) i++; continue; }
        unsigned char c = (unsigned char)s[i];
        if (c < 128) { len += 1; i++; } else { if ((c & 0xF0) == 0xE0) i += 3; else if ((c & 0xE0) == 0xC0) i += 2; else i += 1; len += 2; }
    }
    return len;
}

// --- Modern UI ---
void ui_header(const char* title) {
    printf("\n");
    printf(C_BOLD C_GOLD "   %s \n" C_RESET, title);
    printf(C_ROYAL "   %s \n" C_RESET, UI_LINE_DBL);
}

void ui_line() {
    printf(C_GRAY "   %s \n" C_RESET, UI_LINE_H);
}

void ui_item(const char* text) {
    printf("   " C_ROYAL ICON_ARROW C_RESET " %s\n", text);
}

void ui_title_text(const char* text) {
    printf("   " C_GOLD ICON_BULLET C_RESET " %s\n", text);
}

void print_card_fmt(int lv, char* buffer) {
    const char* style = C_RESET;
    if (lv == 1) style = BG_BRONZE;
    else if (lv <= 3) style = BG_SILVER;
    else if (lv <= 6) style = BG_GOLD;
    else if (lv <= 9) style = BG_DIAMOND;
    else if (lv == 10) style = BG_MASTER;
    sprintf(buffer, "%s +%d %s", style, lv, C_RESET);
}

void print_card_text(int lv, char* buffer) {
    const char* color = C_WHITE;
    if (lv == 1) color = COL_1;
    else if (lv <= 3) color = COL_23;
    else if (lv <= 6) color = COL_46;
    else if (lv <= 9) color = COL_79;
    else if (lv == 10) color = COL_10;
    sprintf(buffer, "%s+%d%s", color, lv, C_RESET);
}

void print_tree(int depth, bool is_end) {
    printf("   "); 
    for (int i = 0; i < depth; i++) {
        printf(C_GRAY " │  " C_RESET);
    }
    if (is_end) printf(C_GRAY " └─ " C_RESET);
    else        printf(C_GRAY " ├─ " C_RESET);
}

void print_indent(int depth) {
    printf("   ");
    for (int i = 0; i < depth; i++) printf(C_GRAY " │  " C_RESET);
    if (depth > 0) printf(C_GRAY " └─ " C_RESET);
    else printf(C_GRAY " ├─ " C_RESET);
}

void play_bar(const char* label, const char* color) {
    int width = 15; 
    int ms = 500; 
    int step = ms/width;
    if (step < 1) step = 1;
    
    printf("%s ", label); printf("\x1b[?25l");
    for(int i=0; i<=width; i++) {
        printf("%s", color);
        for(int j=0; j<width; j++) printf(j<i ? "━" : C_DIM "─"); 
        printf(C_RESET); fflush(stdout);
        if(i<width) { SLEEP_MS(step); for(int k=0; k<width; k++) printf("\b"); }
    }
    printf("\n\x1b[?25h");
}

void pause_key() {
    printf("\n   " C_GRAY ICON_ARROW " 按 Enter 繼續..." C_RESET "\n");
    get_input_int(0);
}

bool wait_action(SimState* st) {
    if (st->operation_mode == 2) { 
        return true; 
    }
    printf("\n   " C_GOLD ">>" C_RESET " [Enter]確認 / [0]回退 / [9]結束");
    printf("\n   " C_GOLD ">> " C_RESET);
    
    char buf[10]; 
    if (fgets(buf, sizeof(buf), stdin)) {
        if (buf[0]=='0') { st->interrupt_flag=true; return false; }
        if (buf[0]=='9') { st->force_exit_flag=true; return false; }
    }
    return true;
}

// ==========================================
//   6. 邏輯與流程
// ==========================================
const char* get_strat_name(int s) {
    if(s==1) return "90% 機率"; if(s==2) return "67% 機率"; return "44% 機率";
}
const char* get_strat_short_name(int s) {
    if(s==1) return "90%"; if(s==2) return "67%"; return "44%";
}
const char* get_op_mode_name(int m) {
    if(m==1) return "手動"; return "全自動";
}
int get_mat_lv(SimState* st, int target) {
    if (st->force_low_lv_use_one && target <= 4) return 1;
    if (st->strategy_mode == 1) return MAX(1, target - 1);
    if (st->strategy_mode == 3) return MAX(1, target - 3);
    return MAX(1, target - 2);
}
int get_req_mat(SimState* st, int target) { return get_mat_lv(st, target); }

void show_inventory(SimState* st) {
    ui_line();
    printf(C_BOLD "   [當前庫存] " C_RESET "\n");
    char l1[600]="", t[100];
    for(int i=1; i<=10; i++) {
        char c[50]; print_card_text(i,c); 
        if (st->inventory[i] > 0) {
            sprintf(t, "%s:%d  ", c, st->inventory[i]); 
            strcat(l1, t);
        }
    }
    if (strlen(l1) == 0) printf("      (無)\n"); else printf("      %s\n", l1);
    printf("      " C_GRAY "欠卡:" C_RESET " %d\n", st->lack_count);
}

void setup_settings(SimState* st) {
    ui_header("系 統 設 定");
    ui_item("[1] 爆擊活動開啟 (1+1, 2+1, 3+1 機率跳級)");
    ui_item("[0] 無活動");
    printf("\n   " C_GOLD ">> " C_RESET);
    int in = get_input_int(0); st->is_event_active=(in==1);
}

void ask_low_level_rule(SimState* st) {
    ui_header("低 階 策 略 (+4以下)");
    ui_item("[1] 強制吃 +1 (省錢)");
    ui_item("[0] 依照設定機率 (如+3吃+3)");
    printf("\n   " C_GOLD ">> " C_RESET);
    int in = get_input_int(1); st->force_low_lv_use_one=(in==1);
}

void setup_stock(SimState* st) {
    ui_header("初 始 庫 存");
    printf(C_GRAY "      %s 設定: %s / %s" C_RESET "\n", ICON_GEAR, get_strat_name(st->strategy_mode), get_op_mode_name(st->operation_mode));
    ui_line();
    ui_title_text("請輸入數量 (直接 Enter 代表 0)");
    printf("\n");
    for(int i=1; i<=9; i++) {
        char c[50]; print_card_text(i, c); 
        printf("      %s : ", c);
        int cnt = get_input_int(0);
        if(cnt > 0) st->inventory[i] = cnt;
    }
}

void buy_one(SimState* st, int depth) {
    if(st->inventory[1]>0) st->inventory[1]--;
    else {
        st->lack_count++;
        print_tree(depth, false); play_bar("補貨", C_CYAN);
        print_tree(depth, true);  printf(C_GOLD "$$" C_RESET " 購入 +1 (欠: %d)\n", st->lack_count);
    }
}

int get_auto_best_stock(SimState* st, int main_lv, int limit_lv) {
    int best = 0, priority = 999;
    for (int i = MAX_LV; i >= limit_lv; i--) {
        if (st->inventory[i] > 0) {
            double rate = PROB_TABLE[main_lv][i]; int p = 4;
            if (fabs(rate-67.68)<0.01) p=1; 
            else if (fabs(rate-44.53)<0.01) p=2; 
            else if (fabs(rate-90.00)<0.01) p=3;
            if (p < priority) { priority = p; best = i; }
        }
    }
    if (best == 0) {
        for (int i = MAX_LV; i >= limit_lv; i--) {
            if (st->inventory[i]>0) { best = i; break; }
        }
    }
    return best;
}

int ask_stock(SimState* st, int main, int req) {
    int min = req+1;
    bool has=false; for(int i=main; i>=min; i--) if(st->inventory[i]>0) has=true;
    if(!has) return 0;

    ui_header("發現高階庫存");
    for(int i=main; i>=min; i--) {
        if(st->inventory[i]>0) {
            char c[50]; print_card_fmt(i, c);
            printf("      %s (%d張) [%.2f%%] -> 輸入 %d\n", c, st->inventory[i], PROB_TABLE[main][i], i);
        }
    }
    ui_line();
    printf("      不使用 -> 輸入 0\n");
    
    if (st->operation_mode == 2) {
        int best = get_auto_best_stock(st, main, min);
        enable_raw_mode();
        bool intr = false;
        printf("\n");
        for(int i=50; i>0; i--) {
            printf("\r   " C_MAGENTA ">> 自動選擇中: %d.%ds (按任意鍵攔截) " C_RESET, i/10, i%10);
            fflush(stdout);
            SLEEP_MS(100);
            if(check_keyboard_hit()) { 
                read_char_no_block(); 
                intr=true; 
                break; 
            }
        }
        disable_raw_mode();
        printf("\n");

        if(!intr) {
            char c[50]; print_card_fmt(best, c);
            printf("   " C_CYAN ">> [自動] 已選用 %s" C_RESET "\n", c);
            return best;
        } else {
            printf("   " C_RED ">> [攔截] 切換至手動選擇..." C_RESET "\n");
        }
    }

    printf("\n   " C_GOLD ">> " C_RESET);
    int choice = get_input_int(0);
    if(choice>0 && choice<=MAX_LV && st->inventory[choice]>0) return choice;
    return 0;
}

bool perform_bg_check(SimState* st, int *curr, int req, int depth) {
    int pick = ask_stock(st, *curr, req);
    if(st->force_exit_flag) return false;
    if(pick>0) {
        st->inventory[pick]--;
        char c1[50], c2[50]; print_card_fmt(*curr, c1); print_card_fmt(pick, c2);
        
        print_tree(depth, false); printf("手動 %s 吃 %s", c1, c2);
        if(!wait_action(st)) { if(!st->force_exit_flag) st->inventory[pick]++; return false; }
        
        print_tree(depth, false); play_bar("強化", C_YELLOW);
        bool suc = (get_rnd_01()*100.0 <= PROB_TABLE[*curr][pick]); bool crit=false;
        if(suc) { if(st->is_event_active && pick==1 && get_rnd_01()<0.5) { *curr+=2; crit=true; } else *curr+=1; }
        
        char cn[50]; print_card_fmt(*curr, cn);
        print_tree(depth, true); 
        if(crit) printf(BG_CRIT_LBL " ★ 爆擊！" C_RESET " %s\n", cn);
        else if(suc) printf(BG_SUCCESS_LBL " ✓ 成功！" C_RESET " %s\n", cn);
        else printf(BG_FAIL_LBL " ✕ 失敗！" C_RESET " %s\n", cn);
        return true;
    }
    return false;
}

void do_synth(SimState* st, int initial_curr, int final_target, int start_depth) {
    SynthFrame stack[100]; 
    int top = 0;
    stack[top++] = (SynthFrame){initial_curr, final_target, start_depth};

    while(top > 0) {
        if (st->force_exit_flag) return;
        SynthFrame* f = &stack[top - 1]; 

        if (f->curr >= f->target) {
            st->inventory[f->curr]++;
            top--;
            continue;
        }

        int req = get_req_mat(st, f->curr + 1);

        if (perform_bg_check(st, &f->curr, req, f->depth)) {
            continue; 
        }
        if (st->force_exit_flag) return;

        if (st->inventory[req] > 0 || req == 1) {
            if (req == 1) buy_one(st, f->depth);
            else { 
                st->inventory[req]--; 
                char c[50]; print_card_fmt(req, c); 
                print_tree(f->depth, false); printf(C_DIM "庫存 %s" C_RESET "\n", c); 
            }
            
            char c1[50], c2[50]; print_card_fmt(f->curr, c1); print_card_fmt(req, c2);
            double rate = PROB_TABLE[f->curr][req];
            
            print_tree(f->depth, false); printf("合成 %s 吃 %s (%.2f%%)", c1, c2, rate);
            
            if (!wait_action(st)) {
                if (st->force_exit_flag) return;
                if (req > 1) st->inventory[req]++; else { st->inventory[1]++; st->lack_count--; }
                print_tree(f->depth, true); printf(C_GRAY "(已退還)" C_RESET "\n");
                continue; 
            }

            print_tree(f->depth, false); play_bar("強化", C_YELLOW);
            
            bool suc = (get_rnd_01() * 100.0 <= rate); bool crit = false;
            if (suc) { 
                if (st->is_event_active && req == 1 && get_rnd_01() < 0.5) { f->curr += 2; crit = true; } 
                else f->curr += 1; 
            }
            
            char cn[50]; print_card_fmt(f->curr, cn);
            print_tree(f->depth, true);
            if (crit) printf(BG_CRIT_LBL " ★ 爆擊！" C_RESET " %s\n", cn);
            else if (suc) printf(BG_SUCCESS_LBL " ✓ 成功！" C_RESET " %s\n", cn);
            else printf(BG_FAIL_LBL " ✕ 失敗！" C_RESET " %s\n", cn);
            
        } else {
            int base = 0;
            for (int lv = req - 1; lv > 1; lv--) {
                if (st->inventory[lv] > 0) {
                    st->inventory[lv]--; base = lv;
                    char c[50]; print_card_fmt(lv, c);
                    print_tree(f->depth + 1, false); printf(C_DIM "庫存 %s (基底)" C_RESET "\n", c);
                    break;
                }
            }
            if (base == 0) { 
                buy_one(st, f->depth + 1); base = 1; 
                if (st->inventory[1] > 0) st->inventory[1]--; 
            }
            stack[top++] = (SynthFrame){base, req, f->depth + 1};
        }
    }
}

void prep_mat(SimState* st, int target, int depth) {
    if(target == 1) { buy_one(st, depth); return; }
    if(st->inventory[target] > 0) return;
    
    int base = 0;
    for(int lv = target - 1; lv > 1; lv--) {
        if(st->inventory[lv] > 0) {
            st->inventory[lv]--; base = lv;
            char c[50]; print_card_fmt(lv, c);
            print_tree(depth, false); printf(C_DIM "庫存 %s (基底)" C_RESET "\n", c);
            break;
        }
    }
    if(base == 0) { 
        buy_one(st, depth); base = 1; 
        if(st->inventory[1] > 0) st->inventory[1]--; 
    }
    do_synth(st, base, target, depth); 
}

void run_sim(SimState* st) {
    setup_settings(st); ask_low_level_rule(st); 
    
    do {
        ui_header("機 率 選 擇");
        ui_item("[1] 90% 成功率");
        ui_item("[2] 67% 成功率");
        ui_item("[3] 44% 成功率");
        printf("\n   " C_GOLD ">> " C_RESET);
        st->strategy_mode = get_input_int(0);
    } while(st->strategy_mode < 1 || st->strategy_mode > 3);

    do {
        ui_header("操 作 模 式");
        ui_item("[1] 手動 (Enter確認/0回退)");
        ui_item("[2] 全自動 (極速/無雜訊)");
        printf("\n   " C_GOLD ">> " C_RESET);
        st->operation_mode = get_input_int(0);
    } while(st->operation_mode < 1 || st->operation_mode > 2);

    setup_stock(st);

    st->lack_count=0; st->force_exit_flag=false;

    int cur=0; for(int i=MAX_LV; i>=1; i--) if(st->inventory[i]>0) { cur=i; break; }
    if(cur==0) { printf("\n   [!] 無庫存, 買入 +1\n"); cur=1; st->lack_count++; }
    else { st->inventory[cur]--; char c[50]; print_card_fmt(cur,c); printf("\n   [!] 取用 %s\n", c); }
    
    st->main_card_lv=cur; show_inventory(st);
    int goal; printf(C_GOLD "\n   目標等級 (%d-10): " C_RESET, cur+1);
    goal = get_input_int(cur+1);
    
    printf("\n" C_CYAN "   設定完成。按 Enter 開始模擬..." C_RESET);
    get_input_int(0);
    clear_screen(); 

    if(goal<=cur) return;

    while(cur < goal) {
        st->main_card_lv = cur; int req = get_mat_lv(st, cur+1);
        if(perform_bg_check(st, &cur, req, 0)) { if(cur>=goal) break; continue; }
        if(st->force_exit_flag) break;

        char t[100]; sprintf(t, "階段: +%d -> +%d", cur, cur+1);
        ui_header(t);
        
        char cr[50]; print_card_fmt(req, cr);
        printf(C_CYAN "      [自動] 準備材料 %s" C_RESET "\n", cr);
        
        if(req>1) { 
            prep_mat(st, req, 1); 
            if(st->force_exit_flag) break; 
            if(perform_bg_check(st, &cur, req, 0)) { if(cur>=goal) break; continue; } 
            if(st->force_exit_flag) break; 
        }
        
        if(st->inventory[req]>0 || req==1) {
            if(req==1) buy_one(st, 0); else { st->inventory[req]--; print_tree(0, false); printf("庫存 %s\n", cr); }
        } else continue;

        char c1[50]; print_card_fmt(cur, c1);
        ui_line();
        printf(C_GOLD "      [決戰] %s 吃 %s (%.2f%%)" C_RESET "\n", c1, cr, PROB_TABLE[cur][req]);
        
        if(!wait_action(st)) {
            if(st->force_exit_flag) break;
            if(req>1) st->inventory[req]++; else { st->inventory[1]++; st->lack_count--; }
            printf(C_GRAY "   [已退還]" C_RESET "\n"); continue;
        }

        play_bar("   強化", C_GOLD);
        bool suc = (get_rnd_01()*100.0 <= PROB_TABLE[cur][req]); bool crit=false;
        if(suc) { if(st->is_event_active && req==1 && get_rnd_01()<0.5) { cur=MIN(10, cur+2); crit=true; } else cur++; }
        
        char cn[50]; print_card_fmt(cur, cn);
        if(crit) printf("   " BG_CRIT_LBL " ★ 爆擊！" C_RESET " 主卡跳級至 %s\n", cn);
        else if(suc) printf("   " BG_SUCCESS_LBL " ✓ 成功！" C_RESET " 主卡升級至 %s\n", cn);
        else printf("   " BG_FAIL_LBL " ✕ 失敗！" C_RESET " 主卡維持 %s\n", cn);
        
        show_inventory(st); if(cur>=goal) break;
    }
    
    ui_header(st->force_exit_flag ? "模 擬 中 止" : "任 務 完 成");
    printf("\n      最終等級: +%d\n", cur);
    printf("      總消耗 +1: %d 張\n", st->lack_count);
    show_inventory(st); pause_key();
}

long long calc_100_path(SimState* st, int lv) {
    if (lv <= 1) return 1;
    int mat = get_mat_lv(st, lv);
    if (st->is_event_active && mat == 1 && lv >= 3) return calc_100_path(st, lv - 2) + calc_100_path(st, 1);
    return calc_100_path(st, lv - 1) + calc_100_path(st, mat);
}

void draw_path(SimState* st, int lv, int depth) {
    if (lv <= 1) return;
    print_indent(depth);
    int mat = get_mat_lv(st, lv);
    if (st->is_event_active && mat == 1 && lv >= 3) {
        printf("[+%d] = [+%d] + [+%d] (跳級)\n", lv, lv-2, mat);
        draw_path(st, lv-2, depth + 1); draw_path(st, mat, depth + 1); return;
    }
    printf("[+%d] = [+%d] + [+%d]\n", lv, lv-1, mat);
    draw_path(st, lv-1, depth + 1); draw_path(st, mat, depth + 1);
}

double calc_avg_cost(SimState* st, int lv, int strat, double* memo) {
    if (lv <= 1) { memo[lv] = 1.0; return 1.0; } if (memo[lv] > 0) return memo[lv];
    int mat = get_mat_lv(st, lv); double cost_mat = calc_avg_cost(st, mat, strat, memo);
    if (st->is_event_active && mat == 1 && lv >= 3) {
        int base_jump = lv - 2; int base_norm = lv - 1; 
        double c_jump = calc_avg_cost(st, base_jump, strat, memo);
        double p_gap = PROB_TABLE[base_norm][mat] / 100.0;
        double p_total = PROB_TABLE[base_jump][mat] / 100.0;
        memo[lv] = c_jump + (cost_mat + 0.5 * (cost_mat/p_gap)) / p_total; 
        return memo[lv];
    }
    double c_base = calc_avg_cost(st, lv - 1, strat, memo);
    double p = PROB_TABLE[lv-1][mat] / 100.0; if (p <= 0) p = 0.01;
    memo[lv] = c_base + (cost_mat / p); return memo[lv];
}

void run_path(SimState* st) {
    setup_settings(st); ask_low_level_rule(st);
    
    do {
        clear_screen();
        ui_header("機 率 選 擇");
        ui_item("[1] 90% 成功率"); ui_item("[2] 67% 成功率"); ui_item("[3] 44% 成功率");
        printf("\n   " C_GOLD ">> " C_RESET); 
        st->strategy_mode = get_input_int(0);
    } while (st->strategy_mode < 1 || st->strategy_mode > 3);

    int s, e; printf("   起點 (1-9): "); s = get_input_int(1);
    printf("   終點 (%d-10): ", s+1); e = get_input_int(s+1);
    long long tot=0; int c=s;
    ui_header("分 析 結 果");
    
    while(c<e) {
        int m=get_mat_lv(st, c+1); long long cost=calc_100_path(st, m);
        int n=(st->is_event_active && m==1)?MIN(e, c+2):c+1;
        printf("      + %d -> +%d (需+%d, 價值%lld)\n", c, n, m, cost);
        draw_path(st, m, 1); c=n; tot+=cost;
    }
    ui_line(); printf("      總計最少: %lld 張 +1\n", tot); pause_key();
}

void run_avg(SimState* st) {
    setup_settings(st); ask_low_level_rule(st);
    int s, e; printf("   起點: "); s = get_input_int(1); printf("   終點: "); e = get_input_int(10);
    ui_header("分 析 結 果");
    for(int i=1; i<=3; i++) {
        double m[MAX_LV+1]={0}; for(int j=1; j<=e; j++) calc_avg_cost(st, j, i, m);
        printf("      %s : %.1f 張\n", (i==1?"90%":i==2?"67%":"44%"), m[e]-m[s]);
    }
    pause_key();
}

int main() {
    init_console(); srand(time(NULL));
    clear_screen();
    
    SimState st;
    memset(&st, 0, sizeof(SimState));
    st.language_mode = 1;
    st.force_low_lv_use_one = true;
    st.strategy_mode = 2;
    st.operation_mode = 1;

    while(true) {
        clear_screen();
        ui_header("SUPER SIM V106.3 (NO JARGON)");
        ui_item("[1] 強化模擬");
        ui_item("[2] 100% 路徑");
        ui_item("[3] 平均花費");
        ui_item("[0] 離開");
        printf("\n   " C_GOLD ">> " C_RESET);
        
        int op = get_input_int(-1);
        if(op==1) run_sim(&st); 
        else if(op==2) run_path(&st); 
        else if(op==3) run_avg(&st); 
        else if(op==0) break;
    }
    return 0;
}