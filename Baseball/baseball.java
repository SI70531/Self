import java.util.Scanner;
import java.util.Random;

public class baseball {
    // --- 顏色定義 ---
    public static final String RESET = "\u001B[0m";
    public static final String BOLD = "\u001B[1m";
    public static final String GRASS_GREEN = "\u001B[32m";
    public static final String DIRT_BROWN = "\u001B[33m";
    public static final String GRAY = "\u001B[90m";
    public static final String CYAN = "\u001B[36m";
    public static final String BG_HOMERUN = "\u001B[45;1;37m";
    public static final String BG_SAFE = "\u001B[42;1;37m";
    public static final String BG_OUT = "\u001B[41;1;37m";

    static int[] locker = new int[11];
    static int totalSpent = 0;
    static int strat = 2;
    static int mode = 1;
    static boolean isEvent = false;
    static boolean forceStop = false;
    
    static Scanner sc = new Scanner(System.in);
    static Random rand = new Random();

    // 成功率表
    static double[][] successRate = {
        {0},
        {0, 90.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
        {0, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
        {0, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
        {0, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0},
        {0, 11.80, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0, 100.0},
        {0, 4.44, 11.80, 25.12, 44.53, 67.68, 90.00, 100.0, 100.0, 100.0, 100.0}
    };

    public static void main(String[] args) {
        while (true) {
            clearScreen();
            uiHeader("⚾ 棒球殿堂：高階卡攔截版 ⚾");
            System.out.println(" [1] 開始強化");
            System.out.println(" [0] 關閉");
            System.out.print("\n >> ");
            String choice = sc.nextLine();
            if (choice.equals("1")) run();
            else if (choice.equals("0")) break;
        }
    }

    public static void run() {
        // 設定流程
        System.out.print(" 大成功加倍? (1:是 / 0:否): ");
        isEvent = sc.nextLine().equals("1");
        System.out.print(" 強化流派 (1:豪華 2:標準 3:省錢): ");
        strat = Integer.parseInt(sc.nextLine());
        System.out.print(" 模式 (1:手動 2:自動): ");
        mode = Integer.parseInt(sc.nextLine());

        inputLocker();

        int playerLv = 0;
        for (int i = 10; i >= 1; i--) if (locker[i] > 0) { playerLv = i; locker[i]--; break; }
        if (playerLv == 0) { playerLv = 1; totalSpent++; }

        System.out.print("\n 目標等級: ");
        int goal = Integer.parseInt(sc.nextLine());

        clearScreen();
        while (playerLv < goal && !forceStop) {
            int req = getNeededMat(playerLv + 1);
            uiHeader("當前球員: +" + playerLv + " -> 目標: +" + goal);
            
            // 關鍵：強化前先問有沒有高階卡可用
            if (checkAndUseHighLvCard(playerLv, req, 0)) {
                // 如果在 check 裡面成功升級了，直接進入下一輪
                playerLv = globalPlayerLv; 
                showLocker();
                continue;
            }

            if (req > 1) prepareMaterial(req, 1);
            if (forceStop) break;

            // 執行主強化
            performMainUpgrade(playerLv, req);
            playerLv = globalPlayerLv;
            showLocker();
        }
        System.out.println("\n 強化結束！消耗 +1 球員: " + totalSpent);
        sc.nextLine();
    }

    static int globalPlayerLv = 0; // 暫存等級用

    // --- 關鍵機制：高階卡詢問 ---
    public static boolean checkAndUseHighLvCard(int currentLv, int minReq, int depth) {
        int bestCard = 0;
        // 尋找倉庫裡比最低需求還高的卡
        for (int i = 10; i > minReq; i--) {
            if (locker[i] > 0) {
                bestCard = i;
                break;
            }
        }

        if (bestCard == 0) return false;

        uiHeader("!!! 發現高階材料卡 !!!");
        System.out.println(" 需求最低 +" + minReq + "，但倉庫有 +" + bestCard + " (成功率 " + successRate[currentLv][bestCard] + "%)");
        
        System.out.print(" 是否直接消耗 +" + bestCard + " 進行強化? (1:確認 / 0:否): ");
        
        boolean useIt = false;
        if (mode == 2) {
            System.out.println("\n [自動模式] 3秒後自動選用...");
            try { Thread.sleep(3000); } catch (Exception e) {}
            useIt = true;
        } else {
            useIt = sc.nextLine().equals("1");
        }

        if (useIt) {
            locker[bestCard]--;
            playBaseballAnim(depth, "高階強化", GRASS_GREEN);
            double rate = (bestCard >= 10) ? 100.0 : successRate[currentLv][bestCard];
            if (rand.nextDouble() * 100 <= rate) {
                globalPlayerLv = currentLv + 1;
                System.out.println(" " + BG_SAFE + " ✓ 成功！ " + RESET + " 升至 +" + globalPlayerLv);
            } else {
                globalPlayerLv = currentLv;
                System.out.println(" " + BG_OUT + " ✕ 失敗！ " + RESET + " 維持 +" + globalPlayerLv);
            }
            return true;
        }
        globalPlayerLv = currentLv;
        return false;
    }

    public static void performMainUpgrade(int playerLv, int matLv) {
        if (locker[matLv] > 0 || matLv == 1) {
            if (matLv == 1) buyStarterCard(0);
            else { locker[matLv]--; printTree(0, false); System.out.println(GRAY + "使用 +" + matLv + RESET); }
            
            double rate = successRate[playerLv][matLv];
            System.out.println(DIRT_BROWN + "   [強化] +" + playerLv + " 吃 +" + matLv + " (" + rate + "%)" + RESET);
            
            if (confirmAction()) {
                playBaseballAnim(0, "揮棒", GRASS_GREEN);
                if (rand.nextDouble() * 100 <= rate) {
                    globalPlayerLv = playerLv + 1;
                    System.out.println(" " + BG_SAFE + " 成功！ +" + globalPlayerLv);
                } else {
                    globalPlayerLv = playerLv;
                    System.out.println(" " + BG_OUT + " 失敗... +" + globalPlayerLv);
                }
            }
        }
    }

    // (其餘 prepareMaterial, playBaseballAnim, printTree 等函式與之前一致)
    // ... [省略重複的輔助函式代碼以節省空間] ...
    
    public static void prepareMaterial(int target, int depth) {
        if (target == 1) { buyStarterCard(depth); return; }
        if (locker[target] > 0) return;
        int start = 1;
        for (int i = target - 1; i > 1; i--) { if (locker[i] > 0) { locker[i]--; start = i; break; } }
        if (start == 1) buyStarterCard(depth);
        int c = start;
        while (c < target && !forceStop) {
            int r = getNeededMat(c + 1);
            if (r > 1 && locker[r] == 0) prepareMaterial(r, depth + 1);
            if (locker[r] > 0 || r == 1) {
                if (r == 1) buyStarterCard(depth);
                else { locker[r]--; printTree(depth, false); System.out.println(GRAY + "消耗 +" + r + RESET); }
                printTree(depth, false);
                System.out.print("製作材料 +" + c + " 餵 +" + r + " ");
                if (confirmAction()) {
                    playBaseballAnim(depth, "練球", CYAN);
                    printTree(depth, true);
                    if (rand.nextDouble() * 100 <= successRate[c][r]) { c++; System.out.println(GRASS_GREEN + "✓ 成功 (+" + c + ")" + RESET); }
                    else System.out.println(ERROR_RED + "✕ 失敗" + RESET);
                }
            }
        }
        locker[target]++;
    }

    public static void buyStarterCard(int d) { totalSpent++; playBaseballAnim(d, "簽約", CYAN); printTree(d, true); System.out.println(DIRT_BROWN + "$$ 簽入+1" + RESET); }
    public static int getNeededMat(int t) { if (t <= 4) return 1; if (strat == 1) return t - 1; return Math.max(1, t - 2); }
    public static void printTree(int d, boolean e) { for (int i = 0; i < d; i++) System.out.print(GRAY + " │  " + RESET); System.out.print(e ? GRAY + " └─ " + RESET : GRAY + " ├─ " + RESET); }
    public static void playBaseballAnim(int d, String l, String c) { int w = 15; for (int i = 0; i <= w; i++) { System.out.print("\r"); for (int k = 0; k < d; k++) System.out.print(GRAY + " │  " + RESET); System.out.print(GRAY + " ├─ " + RESET + l + " " + c); for (int j = 0; j < w; j++) System.out.print(j == i ? "⚾" : "─"); System.out.print(RESET); System.out.flush(); try { Thread.sleep(33); } catch (Exception e) {} } System.out.println(); }
    public static boolean confirmAction() { if (mode == 2) return true; String in = sc.nextLine(); if (in.equals("9")) { forceStop = true; return false; } return true; }
    public static void inputLocker() { System.out.println("\n--- 倉庫清點 ---"); for (int i = 1; i <= 9; i++) { System.out.print(" +" + i + ": "); String s = sc.nextLine(); locker[i] = s.isEmpty() ? 0 : Integer.parseInt(s); } }
    public static void showLocker() { System.out.print(GRAY + " [物資] " + RESET); for (int i = 1; i <= 10; i++) if (locker[i] > 0) System.out.print("+" + i + ":" + locker[i] + " "); System.out.println(); }
    public static void uiHeader(String t) { System.out.println("\n" + BOLD + GRASS_GREEN + " " + t + RESET); System.out.println(GRAY + " ════════════════════════════════════" + RESET); }
    public static void clearScreen() { System.out.print("\033[H\033[J"); System.out.flush(); }
    public static final String ERROR_RED = "\u001B[31m";
}