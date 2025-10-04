#include <windows.h>
#include <stdio.h>




//ֻ��char*�ܴ����ַ�����charֻ�е����ַ�
BYTE keyboardButtonToKeyCode(const char* keyStr) {
    // �����ַ���������ĸ�����֡����ţ�
    if (strlen(keyStr) == 1) {
        char c = keyStr[0];
        switch (c) {
            // ��ĸ��
        case 'A': case 'a': return 0x04;
        case 'B': case 'b': return 0x05;
        case 'C': case 'c': return 0x06;
        case 'D': case 'd': return 0x07;
        case 'E': case 'e': return 0x08;
        case 'F': case 'f': return 0x09;
        case 'G': case 'g': return 0x0A;
        case 'H': case 'h': return 0x0B;
        case 'I': case 'i': return 0x0C;
        case 'J': case 'j': return 0x0D;
        case 'K': case 'k': return 0x0E;
        case 'L': case 'l': return 0x0F;
        case 'M': case 'm': return 0x10;
        case 'N': case 'n': return 0x11;
        case 'O': case 'o': return 0x12;
        case 'P': case 'p': return 0x13;
        case 'Q': case 'q': return 0x14;
        case 'R': case 'r': return 0x15;
        case 'S': case 's': return 0x16;
        case 'T': case 't': return 0x17;
        case 'U': case 'u': return 0x18;
        case 'V': case 'v': return 0x19;
        case 'W': case 'w': return 0x1A;
        case 'X': case 'x': return 0x1B;
        case 'Y': case 'y': return 0x1C;
        case 'Z': case 'z': return 0x1D;

            // ���ּ�������������
        case '1': return 0x27;
        case '2': return 0x28;
        case '3': return 0x29;
        case '4': return 0x2A;
        case '5': return 0x2B;
        case '6': return 0x2C;
        case '7': return 0x2D;
        case '8': return 0x2E;
        case '9': return 0x2F;
        case '0': return 0x30;

            // ���ż�������������
        case '-': case '_': return 0x31;
        case '=': case '+': return 0x32;
        case '[': case '{': return 0x33;
        case ']': case '}': return 0x34;
        case '\\': case '|': return 0x35;
        case ';': case ':': return 0x36;
        case '\'': case '"': return 0x37;
        case '`': case '~': return 0x38;
        case ',': case '<': return 0x39;
        case '.': case '>': return 0x3A;
        case '/': case '?': return 0x3B;

        default: return 0x00;  // δ֪���ַ�
        }
    }
    // ������ַ������
    else {
        if (strcmp(keyStr, "Tab") == 0) return 0x0F;
        if (strcmp(keyStr, "Enter") == 0) return 0x1C;
        if (strcmp(keyStr, "Escape") == 0) return 0x01;
        if (strcmp(keyStr, "Backspace") == 0) return 0x0E;
        if (strcmp(keyStr, "Space") == 0) return 0x39;
        if (strcmp(keyStr, "Shift") == 0) return 0x2A;
        if (strcmp(keyStr, "Ctrl") == 0) return 0x1D;
        if (strcmp(keyStr, "Alt") == 0) return 0x38;
        if (strcmp(keyStr, "F1") == 0) return 0x3B;
        if (strcmp(keyStr, "F2") == 0) return 0x3C;
        if (strcmp(keyStr, "F3") == 0) return 0x3D;
        if (strcmp(keyStr, "F4") == 0) return 0x3E;
        if (strcmp(keyStr, "F5") == 0) return 0x3F;
        if (strcmp(keyStr, "F6") == 0) return 0x40;
        if (strcmp(keyStr, "F7") == 0) return 0x41;
        if (strcmp(keyStr, "F8") == 0) return 0x42;
        if (strcmp(keyStr, "F9") == 0) return 0x43;
        if (strcmp(keyStr, "F10") == 0) return 0x44;
        if (strcmp(keyStr, "F11") == 0) return 0x57;
        if (strcmp(keyStr, "F12") == 0) return 0x58;
        if (strcmp(keyStr, "Insert") == 0) return 0xD2;
        if (strcmp(keyStr, "Delete") == 0) return 0xD3;
        if (strcmp(keyStr, "Home") == 0) return 0xC7;
        if (strcmp(keyStr, "End") == 0) return 0xCF;
        if (strcmp(keyStr, "PageUp") == 0) return 0xC9;
        if (strcmp(keyStr, "PageDown") == 0) return 0xD1;
        if (strcmp(keyStr, "Up") == 0) return 0xC8;
        if (strcmp(keyStr, "Down") == 0) return 0xD0;
        if (strcmp(keyStr, "Left") == 0) return 0xCB;
        if (strcmp(keyStr, "Right") == 0) return 0xCD;

        //Ϊ0x00ʱΪ�ͷ�
        if (strcmp(keyStr, "Null") == 0) return 0x00;

        return 0x00;  // δ֪�����
    }
}


BYTE keyboardModifierToKeyCode(const char* modifierStr) {
    // ��Shift����Shiftͨ���в�ͬ�ļ��룬��������ͬ
    if (strcmp(modifierStr, "Shift") == 0 || strcmp(modifierStr, "LeftShift") == 0)
        return 0x2A;
    if (strcmp(modifierStr, "RightShift") == 0)
        return 0x36;

    // ��Ctrl����Ctrl
    if (strcmp(modifierStr, "Ctrl") == 0 || strcmp(modifierStr, "LeftCtrl") == 0)
        return 0x1D;
    if (strcmp(modifierStr, "RightCtrl") == 0)
        return 0x9D;

    // ��Alt����Alt(Menu��)
    if (strcmp(modifierStr, "Alt") == 0 || strcmp(modifierStr, "LeftAlt") == 0)
        return 0x38;
    if (strcmp(modifierStr, "RightAlt") == 0 || strcmp(modifierStr, "Menu") == 0)
        return 0xB8;

    // Windows��
    if (strcmp(modifierStr, "Win") == 0 || strcmp(modifierStr, "LeftWin") == 0)
        return 0xDB;
    if (strcmp(modifierStr, "RightWin") == 0)
        return 0xDC;

    // �ͷ����μ�
    if (strcmp(modifierStr, "Null") == 0)
        return 0x00;

    return 0x00;  // δ֪���μ�
}

BYTE mouseButtonToKeyCode(const char* buttonStr) {
    // ������
    if (strcmp(buttonStr, "Left") == 0 || strcmp(buttonStr, "left") == 0)
        return 0x01;

    // ����Ҽ�
    if (strcmp(buttonStr, "Right") == 0 || strcmp(buttonStr, "right") == 0)
        return 0x02;

    // ����м�
    if (strcmp(buttonStr, "Middle") == 0 || strcmp(buttonStr, "middle") == 0)
        return 0x04;

    // �����
    if (strcmp(buttonStr, "X1") == 0 || strcmp(buttonStr, "Back") == 0)
        return 0x05;
    if (strcmp(buttonStr, "X2") == 0 || strcmp(buttonStr, "Forward") == 0)
        return 0x06;


    // �ͷ�����
    if (strcmp(buttonStr, "Null") == 0)
        return 0x00;

    return 0x00;  // δ֪����
}


int main() {
    BYTE KeyboardButton = keyboardButtonToKeyCode("F1");
    printf("���̰�����Ӧ�ı�����: 0x%02X\n", KeyboardButton);

    BYTE keyboardModifier = keyboardModifierToKeyCode("Shift");
    printf("�������η���Ӧ�ı�����: 0x%02X\n", keyboardModifier);

    BYTE mouseButtonT = mouseButtonToKeyCode("left");
    printf("��갴����Ӧ�ı�����: 0x%02X\n", mouseButtonT);
    Sleep(2000);
}