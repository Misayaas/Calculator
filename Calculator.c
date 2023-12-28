#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// 定义结构体
typedef struct token
{
    char str[10000]; // 存放语段
    enum             // 标记语段的类型
    {
        NUMBER,
        OPERATOR,
        VARIABLE // NUMBER=0
    } type;
} Token;

typedef struct value
{
    enum // 标记数字的类型
    {
        INT,
        FLOAT,
        ERROR
    } type;
    union // 用于存放具体数
    {
        int iVal;
        double fVal;
    } val;
} Value;

typedef struct variable
{
    char name[10000];
    Value value;
} Variable;

void printfValue(Value res) // 直接处理输出值
{
    switch (res.type)
    {
    case INT:
        printf("%d\n", res.val.iVal);
        break;
    case FLOAT:
        printf("%lf\n", res.val.fVal);
        break;
    case ERROR:
        printf("Error\n");
        break;
    }
}

Token tokens[10000]; // tokens存放分段
Variable var[10000]; // var存放变量
Value error;         // 报错

char put_temp[10000];
int varlen;

// 标注每一个分段的类型
bool Is_Int(char *tokenn) // 判断是不是整形
{
    if (tokenn[0] == '0' && tokenn[1] != '\0')
        return false;

    for (int i = 0; i < strlen(tokenn); i++)
    {
        if (tokenn[i] >= '0' && tokenn[i] <= '9')
            continue;
        else
            return false;
    }
    return true;
}

bool Is_Float(char *tokenn) // 判断是不是浮点数
{
    if (tokenn[0] == '0' && tokenn[1] != '.')
        return false;

    if (strlen(tokenn) == 1)
        return false;

    if (tokenn[0] == '.' || tokenn[strlen(tokenn) - 1] == '.')
        return false;

    int point = 0; // 记录小数点个数
    int num = 0;   // 记录数字的个数
    for (int i = 0; i < strlen(tokenn); i++)
    {
        if (tokenn[i] >= '0' && tokenn[i] <= '9')
            num++;
        else if (tokenn[i] == '.')
            point++;
        else
            return false;
    }

    if (point != 1)
        return false;
    else if (point == 1 && num == strlen(tokenn) - 1)
        return true;
}

bool Is_NUMBER(char *tokenn) // 判断是不是数字
{
    return Is_Int(tokenn) || Is_Float(tokenn);
}

bool Is_Operator(char *tokenn) // 判断是不是操作符
{
    if (strlen(tokenn) != 1)
        return false;

    if (*tokenn == '+' || *tokenn == '-' || *tokenn == '*' || *tokenn == '/' || *tokenn == '=' || *tokenn == '(' || *tokenn == ')')
        return true;
    else
        return false;
}

bool Is_Variable(char *tokenn) // 判断是不是变量
{
    if (tokenn[0] >= '0' && tokenn[0] <= '9')
        return false;

    for (int i = 0; i < strlen(tokenn); i++)
    {
        if (tokenn[i] == '+' || tokenn[i] == '-' || tokenn[i] == '*' || tokenn[i] == '/' || tokenn[i] == '=' || tokenn[i] == '(' || tokenn[i] == ')' || (tokenn[i] >= 'a' && tokenn[i] <= 'z') || (tokenn[i] >= 'A' && tokenn[i] <= 'Z') || tokenn[i] >= '0' && tokenn[i] <= '9')
            continue;
        else
            return false;
    }
    return true;
}

// 判断具体语段属于什么类型
bool judge(char *tokenn, int index) // index作为一个索引
{
    strcpy(tokens[index].str, tokenn); // 存入到结构体中
    if (Is_NUMBER(tokenn) == true)     // 存入类型
    {
        tokens[index].type = NUMBER;
        return true;
    }
    if (Is_Operator(tokenn) == true)
    {
        tokens[index].type = OPERATOR;
        return true;
    }
    if (Is_Variable(tokenn) == true)
    {
        tokens[index].type = VARIABLE;
        return true;
    }
}

int divide() // 找到边界
{
    int len = strlen(put_temp);
    char temp[10000];
    strcpy(temp, put_temp);

    int pointt[10000]; // 将temp中以‘ ’分隔开
    int t = 0;
    pointt[0] = 0;
    for (int i = 0; i < len; i++)
    {
        if (temp[i] == ' ')
        {
            temp[i] = '\0';
            pointt[++t] = i + 1;
        }
    }

    int n = 0;
    for (int i = 0; i <= t; i++)
    {
        if (judge((temp + pointt[i]), ++n) == 0)
            return 0;
    }
    return n;
}

void store(char *var_name, Value value) // 将变量值存入
{
    bool flag = false;
    for (int i = 0; i < varlen; i++)
    {
        if (strcmp(var_name, var[i].name) == 0) // 如果变量名和即存匹配，则将对应的值存入
        {
            flag = true;
            var[i].value = value;
            break;
        }
    }

    if (flag == false) // 否则将所有需要的都存入
    {
        strcpy(var[varlen].name, var_name);
        var[varlen].value = value;
        varlen++;
    }
}

Value fetch(char *var_name) // 根据变量名的匹配来取得对应的值
{
    for (int i = 0; i < varlen; i++)
    {
        if (strcmp(var_name, var[i].name) == 0)
            return var[i].value;
    }
    return error;
}

Value eval_Number(Token tokenn) // 将整形和浮点型分出并存好
{
    Value TEMP;
    if (Is_Int(tokenn.str) == true) // 在不影响原结构体的情况下准备替代结构体进行运算
    {
        TEMP.type = INT;
        sscanf(tokenn.str, "%d", &TEMP.val.iVal);
    }
    else if (Is_Float(tokenn.str) == true)
    {
        TEMP.type = FLOAT;
        sscanf(tokenn.str, "%lf", &TEMP.val.fVal);
    }
    return TEMP;
}

int find_Operator(int l, int r) // 模拟栈
{
    int index = -1;
    int ret = 0; // 判断操作符位置
    bool flag = true;

    for (int i = l; i <= r; i++)
    {
        if (tokens[i].type != OPERATOR)
        {
            flag = false;
            continue;
        }
        else // 万事先配括号
        {
            if (tokens[i].str[0] == '(')
                index++;
            else if (tokens[i].str[0] == ')')
            {
                if (index == -1)
                    return 0;

                index--;
                flag = false;
            }

            if (index != -1)
                continue;

            // 防止flag默认变为true
            if (tokens[i].str[0] != '+' && tokens[i].str[0] != '-' && tokens[i].str[0] != '*' && tokens[i].str[0] != '/')
                continue;

            if (tokens[i].str[0] == '+' || (tokens[i].str[0] == '-' && flag == false)) // 取第一个减号（为负号做准备）
                ret = i;
            else if (tokens[i].str[0] == '*' || tokens[i].str[0] == '/') // 处理计算优先级
            {
                if (ret == 0 || tokens[ret].str[0] == '*' || tokens[ret].str[0] == '/')
                    ret = i;
            }
            flag = true;
        }
    }
    if (index != -1)
        return 0;

    return ret;
}

Value eval_Token(Token tokenn) // 结合以上两个函数处理每个分段
{
    if (tokenn.type == VARIABLE)
        return fetch(tokenn.str);

    return eval_Number(tokenn);
}

Value meetvalue(Value val_1, Value val_2, int p)
{
    Value ret;

    if (tokens[p].str[0] == '=' || val_1.type == ERROR || val_2.type == ERROR) // error条件
        return error;

    if (val_1.type != val_2.type) // 将不同类型的值对应好 进行下面的结合计算
    {
        if (val_1.type == INT && val_2.type == FLOAT) // 只能增精度（int ——>float）
        {
            val_1.type = FLOAT;
            val_1.val.fVal = (double)val_1.val.iVal;
        }
        else if (val_1.type == FLOAT && val_2.type == INT)
        {
            val_2.type = FLOAT;
            val_2.val.fVal = (double)val_2.val.iVal;
        }
    }

    ret.type = val_1.type;
    if (val_1.type == INT) // 底层的运算 构筑下面的eval函数
    {
        if (tokens[p].str[0] == '+')
            ret.val.iVal = val_1.val.iVal + val_2.val.iVal;
        else if (tokens[p].str[0] == '-')
            ret.val.iVal = val_1.val.iVal - val_2.val.iVal;
        else if (tokens[p].str[0] == '*')
            ret.val.iVal = val_1.val.iVal * val_2.val.iVal;
        else if (tokens[p].str[0] == '/')
            ret.val.iVal = val_1.val.iVal / val_2.val.iVal;
    }
    else if (val_1.type == FLOAT)
    {
        if (tokens[p].str[0] == '+')
            ret.val.fVal = val_1.val.fVal + val_2.val.fVal;
        else if (tokens[p].str[0] == '-')
            ret.val.fVal = val_1.val.fVal - val_2.val.fVal;
        else if (tokens[p].str[0] == '*')
            ret.val.fVal = val_1.val.fVal * val_2.val.fVal;
        else if (tokens[p].str[0] == '/')
            ret.val.fVal = val_1.val.fVal / val_2.val.fVal;
    }
    return ret;
}

// 减号和负数的讨论
bool check_minus(int l, int r) // 检查语段最前面是否有minus（实际上所有的负号都能由操作符切割转化为这种形式）
{
    if (tokens[l].type != OPERATOR || tokens[l].str[0] != '-')
        return false;

    bool flag = true; // 模拟栈（配括号）
    int index = -1;
    for (int i = l; i <= r; i++)
    {
        if (tokens[i].type != OPERATOR)
        {
            flag = false;
            continue;
        }

        if (tokens[i].str[0] == '(')
        {
            index++;
        }
        else if (tokens[i].str[0] == ')')
        {
            if (index == -1)
            {
                return false;
            }
            index--;
            flag = false;
        }

        if (index != -1)
            continue;

        if (tokens[i].str[0] == '+' || tokens[i].str[0] == '*' || tokens[i].str[0] == '/')
            return false;
        else if (tokens[i].str[0] == '-' && flag == false)
            return false;
        else if (tokens[i].str[0] != '+' && tokens[i].str[0] != '-' && tokens[i].str[0] != '*' && tokens[i].str[0] != '/')
            continue;

        flag = true;
    }
    return true;
}

Value MinusEval(Value tokenn) // 遇到minus时对存储的值做相应的改变
{
    Value ret = tokenn;
    if (tokenn.type == ERROR)
        return error;
    else if (tokenn.type == INT)
        ret.val.iVal *= -1;
    else if (tokenn.type == FLOAT)
        ret.val.fVal *= -1;

    return ret;
}

bool all_parentheses(int l, int r) // 模拟栈实现整体括号匹配,如果最外层有括号先去掉
{
    if (tokens[l].type != OPERATOR || tokens[r].type != OPERATOR || tokens[l].str[0] != '(' || tokens[r].str[0] != ')')
        return false;

    int index = -1;
    for (int i = l; i <= r; i++)
    {
        if (tokens[i].type == OPERATOR)
        {
            if (tokens[i].str[0] == '(')
                index++;
            else if (tokens[i].str[0] == ')')
            {
                if (index == -1 || (index == 0 && i != r))
                    return false;
                index--;
            }
        }
    }
    return true;
}

bool part_parentheses(int l, int r) // 判断分式里的小括号是否匹配
{
    int index = -1;
    for (int i = l; i <= r; i++)
    {
        if (tokens[i].type == OPERATOR)
        {
            if (tokens[i].str[0] == '(')
                index++;
            else if (tokens[i].str[0] == ')')
            {
                if (index == -1)
                    return false;
                index--;
            }
        }
    }
    if (index == -1)
        return true;
    else
        return false;
}

int Assign_Position(int l, int r) // 确定赋值的位置
{
    for (int i = l; i <= r; i++)
    {
        if (tokens[i].type == OPERATOR)
        {
            if (tokens[i].str[0] == '=')
                return i;
        }
    }
    return 0;
}

// 终于可以开始计算哩
Value eval(int l, int r)
{
    if (part_parentheses(l, r) == 0 || Assign_Position(l, r) != 0) // 判断是否匹配和赋值位置是否正确
        return error;

    if (l > r)
        return error;

    if (l == r)
    {
        if (tokens[l].type == OPERATOR)
            return error;

        return eval_Token(tokens[l]); // 直接对该字符进行处理
    }

    else if (all_parentheses(l, r) == true) // 删除外面的大括号
        return eval(l + 1, r - 1);

    else if (check_minus(l, r) == true) // 检查最前面的负号
    {
        return MinusEval(eval(l + 1, r));
    }
    else
    {
        if (find_Operator(l, r) == false) // 不出现操作符直接做掉
            return error;

        Value val_1, val_2;
        val_1 = eval(l, find_Operator(l, r) - 1); // 从操作符前后分段操作
        val_2 = eval(find_Operator(l, r) + 1, r);

        return meetvalue(val_1, val_2, find_Operator(l, r)); // 最后计算
    }
}

Value VariableAssign(int l, int r) // 变量赋值函数
{
    if (l > r)
        return error;

    if ((Assign_Position(l, r) != l + 1 || tokens[l].type != VARIABLE) && Assign_Position(l, r) != 0)
        return error;
    else if (Assign_Position(l, r) == 0)
        return eval(l, r);

    Value ret = VariableAssign(Assign_Position(l, r) + 1, r); // 赋值号右边递归
    if (ret.type != ERROR)
        store(tokens[l].str, ret);

    return ret;
}

int main(void)
{
    error.type = ERROR; // error用于输出
    while (gets(put_temp) != NULL)
    {
        int right = divide();
        if (right == 0)
            printf("Error\n");
        else
        {
            if (Assign_Position(1, right) == 0) // 纯数字
                printfValue(eval(1, right));
            else
                printfValue(VariableAssign(1, right)); // 变量
        }
    }
    return 0;
}
