#include <iostream>
#include <cmath>
using namespace std;
int main()
{
    double a, c, b, D;
    cout << "Put in coefficients in order A, B, C:\n";
    cin >> a >> b >> c;
    if (a == 0 && b == 0)
    {
        cout << "it makes no sense\n";
        return 0;
    }
    cout << "Your answer is:\n";
    if (a == 0)
    {
        cout << -c / b << "\n";
        return 0;
    }
    if (fmod(b, 2) != 0)
    {
        D = b * b - 4 * a * c;
        if (D >= 0)
        {
            D = sqrt(D);
            double x1 = (-b + D) / (2 * a);
            double x2 = (-b - D) / (2 * a);
            cout << x1 << " " << x2 << "\n";
            return 0;
        }
    }
    else
    {
        b = b / 2;
        D = b * b - a * c;
        if (D >= 0)
        {
            D = sqrt(D);
            double x1 = (-b + D) / a;
            double x2 = (-b - D) / a;
            cout << x1 << " " << x2 << "\n";
            return 0;
        }
    }
    cout << "No answer. D < 0\n";
    return 0;
}
