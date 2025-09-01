#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
using namespace std;

int main() {
    int a[2], b[2];
    pipe(a);
    pipe(b);

    pid_t p = fork();
    const int n = 1000000;
    timeval t1{}, t2{};

    if (p < 0) {
        cout << "error: fork fail\n";
        return 1;
    } else if (p == 0) {
        int x;
        for (int i = 0; i < n; ++i) {
            if (read(a[0], &x, sizeof(int)) != sizeof(int)) {
                cout << "err: read\n";
                break;
            }
            if (write(b[1], &x, sizeof(int)) != sizeof(int)) {
                cout << "err: write\n";
                break;
            }
        }
    } else {
        int x;
        gettimeofday(&t1, nullptr);

        for (int i = 0; i < n; ++i) {
            x = i;
            if (write(a[1], &x, sizeof(int)) != sizeof(int)) {
                cout << "err: send\n";
                break;
            }
            if (read(b[0], &x, sizeof(int)) != sizeof(int)) {
                cout << "err: recv\n";
                break;
            }
        }

        gettimeofday(&t2, nullptr);

        double tt = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;

        cout << "time: " << tt << " s\n";
        cout << "rps: " << (n / tt) << "\n";
        cout << "avg rt: " << (tt / n) * 1e6 << " us\n";
        cout << "1 way: " << ((tt / n) * 1e6) / 2 << " us\n";

        wait(nullptr);
    }

    return 0;
}
