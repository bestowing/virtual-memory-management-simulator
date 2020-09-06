#include "main.h"

int     set_simulation(void);
int     init_ptr(void);
void    init_frame(void);

int N;              // process가 갖는 page 개수 (10개 내외; 5 ~ 15), 0번부터 시작
int M;              // 할당 page frame 개수 (고정 할당의 경우에만 사용)
int W;              // window size (WS 기법에서만 사용)
int K;              // page reference string 길이
int *pg_ref;        // page reference string을 저장하는 포인터
int *memory;        // 메모리 상태를 저장하는 포인터
int *timestamp1;    // page memory load time을 저장하는 포인터
int *timestamp2;    // last page reference time
int *reference_cnt; // page reference count를 저장하는 포인터

// 콘솔에서 사용자로부터 정보를 입력받음
// 입력받은 정보를 전역변수와 동적할당한 배열에 저장함
int set_simulation(void) {
    if (scanf("%d %d %d %d", &N, &M, &W, &K) != 4)
    {
        printf("Error code 01: failed to receive user input.\n");
        return (-1);
    }
    if (init_ptr() == -1)
    {
        printf("Error code 02: failed to allocate memory bytes.\n");
        return (-1);
    }
    init_frame();
    return (0);
}

int init_ptr(void)
{
    pg_ref = (int*)malloc(sizeof(int) * K);
    if (pg_ref == NULL)
        return (-1);
    memory = (int*)malloc(sizeof(int) * M);
    if (memory == NULL)
        return (-1);
    timestamp1 = (int*)malloc(sizeof(int) * M);
    if (timestamp1 == NULL)
        return (-1);
    timestamp2 = (int*)malloc(sizeof(int) * M);
    if (timestamp2 == NULL)
        return (-1);
    reference_cnt = (int*)malloc(sizeof(int) * N);
    if (reference_cnt == NULL)
        return (-1);
    for(int i=0; i<K; i++) {
        scanf("%d", &pg_ref[i]);
    }
    return (0);
}

// page frame을 초기 세팅하는 함수
// 해당 frame에 loading된 page number를 저장
void init_frame(void) {
    for(int i=0; i<M; i++) {
        memory[i] = -1;    // -1로 초기화: load된 page가 없음을 표현
        timestamp1[i] = 0; // page memory load time 초기화
        timestamp2[i] = 0; // last page reference time 초기화
    }
    for(int i=0; i<N; i++) {
        reference_cnt[i] = 0; // page reference count 초기화
    }
    return ;
}