#include<iostream>
#include<vector>
#include<cmath>
using namespace std;

int main() {
    int mm = 1;
    double cur2 = 0;
    double ua = 0;

    while (mm) {
        int a;
        cout << "1是不确定度A的估算，2是不确定度B的估算" << endl;
        cout << "输入一个数字吧: ";
        cin >> a;

        switch (a) {
            case 1: {
                char s;
                int i = 0;
                bool n = true;
                vector<double> pass;
                double sum = 0;

                cout << "输入数据，输入w结束输入" << endl;
                double x = 0;

                while (n == true) {
                    cout << "输入数据: ";
                    if (!(cin >> x)) {  // 检查输入是否有效
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "输入无效，请重新输入" << endl;
                        continue;
                    }

                    pass.push_back(x);
                    i++;
                    sum += x;

                    cout << "继续输入吗？输入w结束，其他键继续: ";
                    cin >> s;
                    if (s == 'w') {
                        n = false;
                    } else {
                        n = true;
                    }
                }

                if (i > 0) {
                    double average = sum / i;
                    double cursum = 0;
                    for (int j = 0; j < i; j++) {
                        cursum += (pass[j] - average) * (pass[j] - average);
                    }
                    if (i > 1) {
                        double avg = cursum / (i * (i - 1));
                        ua = sqrt(avg);
                        cout << "UA是: " << ua << endl;
                    } else {
                        cout << "数据不足，无法计算UA" << endl;
                    }
                } else {
                    cout << "没有输入数据" << endl;
                }
                break;
            }
            case 2: {
                double cc;
                cout << "输入误差值: ";
                cin >> cc;
                double cur1 = sqrt(3);
                cur2 = cc / cur1;
                cout << "UB是: " << cur2 << endl;
                break;
            }
            default:
                cout << "error: 请输入1或2" << endl;
        }

        cout << "输入0退出，其他数字继续计算: ";
        cin >> mm;
    }

    int wzb;
    cout << "1是单不确定度，2是多不确定度合成: ";
    cin >> wzb;

    switch (wzb) {
        case 1: {
            if (ua != 0 || cur2 != 0) {
                double result = sqrt(cur2 * cur2 + ua * ua);
                cout << "合成不确定度是: " << result << endl;
            } else {
                cout << "请先计算UA和UB" << endl;
            }
            break;
        }
        case 2: {
            int ss = 1;
            double cursumm = 0;
            while (ss) {
                cout << "输入UA: ";
                double cur4 = 0;
                cin >> cur4;
                cursumm += cur4 * cur4;
                cout << "继续输入UA吗？0结束，其他数字继续: ";
                cin >> ss;
            }

            int d = 1;
            double cursumm1 = 0;
            while (d) {
                cout << "输入UB: ";
                double cur4 = 0;
                cin >> cur4;
                cursumm1 += cur4 * cur4;
                cout << "继续输入UB吗？0结束，其他数字继续: ";
                cin >> d;
            }

            double res = sqrt(cursumm1 + cursumm);
            cout << "合成不确定度结果是: " << res << endl;
            break;
        }
        default:
            cout << "无效选择" << endl;
    }

    cout << "按回车键退出...";
    cin.ignore();
    getchar();
    return 0;
}
