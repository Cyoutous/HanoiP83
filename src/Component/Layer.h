#pragma once

struct Layer {
    int value = 0;
};

/*
Layer 值越大越靠前（sort 时 low→high，后画的在上面）

6  遮罩
5  视窗子按钮（X 按钮）
4  视窗面板
3  底部按钮栏 / 数字视窗 / 指示灯 / 计数器 / 关闭按钮
2  盘子
1  柱子
0  背景实体

*/