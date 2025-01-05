# Pipeline Simulation Program

## 概述
本程式模擬了一個簡單的指令流水線執行過程，主要實現對常見指令的處理，包括：
- add
- sub
- lw
- sw
- beq

並生成執行過程的詳細輸出，方便分析和觀察指令的執行順序與流水線行為。

## 文件結構
- `../inputs/test3.txt`：指令輸入文件3。
- `../inputs/test4.txt`：指令輸入文件4。
- `../inputs/test5.txt`：指令輸入文件5。
- `../inputs/test6.txt`：指令輸入文件6。
- `../outputs/test3.txt`：執行輸出文件3。
- `../outputs/test4.txt`：執行輸出文件4。
- `../outputs/test5.txt`：執行輸出文件5。
- `../outputs/test6.txt`：執行輸出文件6。

## 使用方法
1. **準備輸入文件**
   - 在 `../inputs/` 資料夾中準備格式正確的輸入文件，每行為一條指令。
   - 指令支持格式：
     - `lw $rt, offset($rs)`
     - `sw $rt, offset($rs)`
     - `add $rd, $rs, $rt`
     - `sub $rd, $rs, $rt`
     - `beq $rs, $rt, offset`

2. **編譯與執行**
   - 編譯程式：
     ```bash
     g++ -o pipeline_simulation main.cpp
     ```
   - 執行程式：
     ```bash
     ./pipeline_simulation
     ```

3. **查看輸出**
   - 執行結果會輸出到 `../outputs/` 資料夾中對應的文件。

## 程式功能詳解
### 主函數
- `main()`:
  - 調用 `every_thing()` 處理四組輸入文件，並將結果輸出到對應的輸出文件中。

### 關鍵函數
1. `fcinp()`:
   - 解析每條指令的參數。
   - 依據不同指令類型（lw、sw、add、sub、beq），從輸入流中提取必要的操作數和偏移量。

2. `every_thing()`:
   - 實現指令的模擬執行，模擬流水線中各階段的行為（如 IF、ID、EX、MEM、WB）。
   - 根據指令類型執行對應操作，並處理流水線中的冒險（如資料冒險、控制冒險）。
   - 將結果記錄到輸出文件中，包括：
     - 每個指令在各個周期的狀態。
     - 寄存器和記憶體的最終狀態。
     - 指令執行的完整周期數。

## 輸出內容
- 流水線各周期的執行情況。
- 記憶體（mem[32]）與寄存器（reg[32]）的最終內容。
- 指令執行的詳細流水線圖表。

## 注意事項
1. 確保輸入文件格式正確，否則會導致解析錯誤。
2. 寄存器和記憶體初始值均為 1（除了寄存器 `$0` 為固定值 0）。
3. 最大支持 20 條指令，若超過可能導致未定義行為。(若需要可自行增加)

## 示例輸入與輸出
### 示例輸入 (`test3.txt`):
```plaintext
lw $2, 8($0)
lw $3, 16($0) 
beq $2, $3, 1
add $4, $2, $3
sw $4, 24($0)
```

### 對應輸出 (`test3.txt`):
```plaintext
cycle 1
    lw : if 
cycle 2
    lw : id 
    lw : if 
cycle 3
    lw : ex  01 010 11
    lw : id 
    beq: if 
cycle 4
    lw : me  010 11
    lw : ex  01 010 11
    beq: id 
    add: if 
cycle 5
    lw : wb  11
    lw : me  010 11
    beq: id 
    add: if 
cycle 6
    lw : wb  11
    beq: id 
    add: if 
cycle 7
    beq: ex  x0 100 0x
    sw : if 
cycle 8
    beq: me  100 0x
    sw : id 
cycle 9
    beq: wb  0x
    sw : ex  x1 001 0x
cycle 10
    sw : me  001 0x
cycle 11
    sw : wb  0x

11 cycles

$1  $2  $3  $4  $5  $6  $7  $8  $9  $10 $11 $12 $13 $14 $15 $16 $17 $18 $19 $20 $21 $22 $23 $24 $25 $26 $27 $28 $29 $30 $31 $32
1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   
w1  w2  w3  w4  w5  w6  w7  w8  w9  w10 w11 w12 w13 w14 w15 w16 w17 w18 w19 w20 w21 w22 w23 w24 w25 w26 w27 w28 w29 w30 w31 w32
0   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   

    1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20
lw  if  id  ex  me  wb                          
lw      if  id  ex  me  wb                      
beq         if  id  id  id  ex  me  wb          
add             if  if  if                      
sw                          if  id  ex  me  wb
```
