#include "main.h"

void print_table_Top(int isFixed, int i);
void print_table_Content_fixed(int time, int page_string, int isLoaded);
void print_table_Content_vari(int time, int page_string, int isLoaded, int out_page, int * working_set);
void print_table_Bottom(int isFixed, int page_fault);
void resource_return(void);

extern int N;              // process가 갖는 page 개수 (10개 내외; 5 ~ 15), 0번부터 시작
extern int M;              // 할당 page frame 개수 (고정 할당의 경우에만 사용)
extern int *pg_ref;        // page reference string을 저장하는 포인터
extern int *memory;        // 메모리 상태를 저장하는 포인터
extern int *timestamp1;    // page memory load time을 저장하는 포인터
extern int *timestamp2;    // last page reference time
extern int *reference_cnt; // page reference count를 저장하는 포인터

// 시뮬레이션 결과를 출력하기 위한 함수 (1)
// Table의 위쪽 design을 출력함
void print_table_Top(int isFixed, int i) {
    if( isFixed == true ) {
        if( i == MIN ) {
            printf("MIN\n");
        } else if ( i == FIFO ) {
            printf("FIFO\n");
        } else if ( i == LRU ) {
            printf("LRU\n");
        } else if ( i == LFU ) {
            printf("LFU\n");
        } else { }
        printf("+------+-----+-----+\n");
        printf("| Time | str | P.f | Memory\n");
        printf("+------+-----+-----+\n");
    } else {
        printf("WS\n");
        printf("+------+-----+-----+-----+\n");
        printf("| Time | str | P.f | out | Memory\n");
        printf("+------+-----+-----+-----+\n");
    }
    return;
}

// 시뮬레이션 결과를 출력하기 위한 함수 (2) - fixed allocation
// Table의 내용을 출력함
void print_table_Content_fixed(int time, int page_string, int isLoaded) {
    int asc = (isLoaded == true) ? 32 : 70;
    printf("| %4d | %3d | %2c  | ", time, page_string, asc);

    for(int j=0; j<M; j++) {
        if( memory[j] == -1 ) {
            printf("  ");
        } else {
            printf("%2d ", memory[j]);
        }
    }
    
    printf("\n");
    return;
}

// 시뮬레이션 결과를 출력하기 위한 함수 (2) - variable allocation
// Table의 내용을 출력함
void print_table_Content_vari(int time, int page_string, int isLoaded, int out_page, int * working_set) {
    int asc = (isLoaded == true) ? 32 : 70;
    if(out_page == -1) {
        char out = 32;
        printf("| %4d | %3d | %2c  | %2c  | ", time, page_string, asc, out);
    } else {
        printf("| %4d | %3d | %2c  | %2d  | ", time, page_string, asc, out_page);

    }
    for(int i=0; i<N; i++) {
        if( working_set[i] == true ) {
            printf("%2d ", i);
        }
    }
    printf("\n");
    return;
}

// 시뮬레이션 결과를 출력하기 위한 함수 (3)
// Table의 아래쪽 design을 출력함
void print_table_Bottom(int isFixed, int page_fault) {
    if(isFixed == true) {
        printf("+------+-----+-----+\n");
    } else {
        printf("+------+-----+-----+-----+\n");
    }
    printf("Total page fault: %d\n\n\n", page_fault);
    return;
}

// 동적할당한 메모리를 반환함
void resource_return(void) {
    free(pg_ref);
    free(memory);
    free(timestamp1);
    free(timestamp2);
    free(reference_cnt);
    return ;
}