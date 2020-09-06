#include "main.h"

int     set_simulation(void);
void    start_simulation(void);
void    resource_return(void);

int main(int argc, char *argv[]) {
    if (set_simulation() == -1)    // 시뮬레이션 세팅
        return (0);
    start_simulation();  // 시뮬레이션 시작
    resource_return();   // 자원 반납
    return (0);
}