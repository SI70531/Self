import sys
import time

def blackjack_analysis():
    print("\n--- [21點：大數法則與算牌分析] ---")
    print("正在初始化 6 副牌組環境...")
    # 這裡之後會放入你的 Monte Carlo 邏輯
    time.sleep(1)
    print("目前功能：模擬基本策略勝率、凱利公式下注建議。")
    input("\n(按 Enter 回到主選單)")

def holdem_analysis():
    print("\n--- [德州撲克：蒙地卡羅勝率計算] ---")
    print("正在啟動 Equity 計算引擎...")
    # 這裡之後會放入你的 Equity 計算邏輯
    time.sleep(1)
    print("目前功能：起手牌勝率分析、底池賠率 EV 計算。")
    input("\n(按 Enter 回到主選單)")

def main_menu():
    while True:
        print("\n" + "="*30)
        print("   SI 專業博弈分析終端 v1.0")
        print("="*30)
        print(" 1. 開始 21點 (Blackjack) 模擬")
        print(" 2. 開始 德州撲克 (Hold'em) 模擬")
        print(" 3. 退出系統")
        print("-" * 30)
        
        choice = input("請輸入序號 (1-3): ").strip()
        
        if choice == '1':
            blackjack_analysis()
        elif choice == '2':
            holdem_analysis()
        elif choice == '3':
            print("正在關閉系統... 祝你期望值永遠為正！")
            sys.exit()
        else:
            print("輸入錯誤，請輸入 1, 2 或 3。")

if __name__ == "__main__":
    main_menu()