#include "main.h"

void start_simulation(void);
void fixed_allo(int replacement);
void variable_allo(int replacement);
int MIN_algo(int time);
int FIFO_algo(void);
int LRU_algo(void);
int LFU_algo(void);

extern void print_table_Top(int isFixed, int i);
extern void print_table_Content_fixed(int time, int page_string, int isLoaded);
extern void print_table_Content_vari(int time, int page_string, int isLoaded, int out_page, int * working_set);
extern void print_table_Bottom(int isFixed, int page_fault);

extern int N;              // process가 갖는 page 개수 (10개 내외; 5 ~ 15), 0번부터 시작
extern int M;              // 할당 page frame 개수 (고정 할당의 경우에만 사용)
extern int W;              // window size (WS 기법에서만 사용)
extern int K;              // page reference string 길이
extern int *pg_ref;        // page reference string을 저장하는 포인터
extern int *memory;        // 메모리 상태를 저장하는 포인터
extern int *timestamp1;    // page memory load time을 저장하는 포인터
extern int *timestamp2;    // last page reference time
extern int *reference_cnt; // page reference count를 저장하는 포인터

void start_simulation(void) {
    // Fixed allocation based replacement
    // MIN, FIFO, LRU, LFU replacement strategy 적용하여 시뮬레이션 실시
    fixed_allo(MIN);
    fixed_allo(FIFO);
    fixed_allo(LRU);
    fixed_allo(LFU);

    // Variable allocation based replacement
    // WS replacement strategy 적용하여 시뮬레이션 실시
    variable_allo(WS);
    return ;
}

// replacement 기법을 입력받아 시뮬레이션을 실시하는 함수
// Fixed allocation
void fixed_allo(int replacement) {
    print_table_Top(true, replacement);    
    // page fault 발생 횟수
    int page_fault = 0;

    // 모든 page reference string을 확인
    for(int i=0; i<K; i++) {
        int time = i+1;         // 현재 시각
        int page = pg_ref[i];  // 현재 참조할 page number
        int isLoaded = false;  // 메모리에 있는지 확인
        reference_cnt[page] += 1;  // 참조된 횟수 추가

        for(int j=0; j<M; j++) {
            // 이미 메모리에 load된 page인 경우
            if( memory[j] == page ) {
                
                timestamp2[j] = time; // 참조된 시간 최신화
                isLoaded = true;
                break;
            }
        }

        // 메모리에 load되지 않은 page -> Page fault 발생
        if(isLoaded == false) {
            page_fault += 1;      // Page fault count increase
            int position = -1;    // load할 위치
            int isEmpty = false;  // 메모리에 빈 공간이 있는지 확인

            // 빈 frame을 찾음
            for(int j=0; j<M; j++) {
                if( memory[j] == -1 ) {
                    position = j;
                    isEmpty = true;
                    break;
                }
            }

            // frame에 빈 공간이 없으면, replacement 기법에 따라 victim을 찾음
            if(isEmpty == false) {
                switch (replacement) {
                case MIN:
                    position = MIN_algo(i);
                    break;
                case FIFO:
                    position = FIFO_algo();
                    break;
                case LRU:
                    position = LRU_algo();
                    break;
                case LFU:
                    position = LFU_algo();
                    break;
                default:    // 예외상황
                    printf("ERROR: wrong replacement strategy detected.\n");
                    return; // 바로 종료
                }
            }

            memory[position] = page;    // 찾은 position에 page를 load함
            timestamp1[position] = time; // load된 시간 초기화
            timestamp2[position] = time; // 참조된 시간 초기화
        }

        print_table_Content_fixed(i+1, page, isLoaded);
    }

    print_table_Bottom(true, page_fault);
    return;
}

// replacement 기법을 입력받아 시뮬레이션을 실시하는 함수
// Variable allocation
void variable_allo(int replacement) {
    print_table_Top(false, replacement);

    // working set을 담는 배열
    int *working_set = (int*)malloc(sizeof(int) * N);
    for(int z=0; z<N; z++) working_set[z] = false;
    
    // page fault 발생 횟수
    int page_fault = 0;

    // 모든 page reference string을 확인
    for(int i=0; i<K; i++) {
        int page = pg_ref[i];  // 현재 참조할 page number
        int isLoaded = false;  // 메모리에 있는지 확인
        int out_page = -1;

        // window size만큼 reference string에서 working set을 찾는다
        
        for(int j=0; j<N; j++) if(working_set[j] == true) working_set[j] = -1;
    
        // window size만큼 반복함
        for(int z=1; z<=W; z++) {
            // index 검사
            if( i-z < 0 ) break;

            int str = pg_ref[i-z];
            working_set[str] = true;
        }

        // 현재 string이 working set에 있는지 확인
        if( working_set[page] != false ) {
            isLoaded = true;
        } else {
            // 없으면 page fault 발생
            page_fault += 1;
        }
        
        // working set에 현재 string을 넣는다
        working_set[page] = true;

        for(int j=0; j<N; j++) {
            if(working_set[j] == -1) {
                out_page = j;
                working_set[j] = false;
            }
        }

        print_table_Content_vari(i+1, page, isLoaded, out_page, working_set);
    }
    print_table_Bottom(false, page_fault);
    free(working_set);
    
    return;
}

// MIN algorithm 적용
// victim의 page number를 반환함
int MIN_algo(int time) {
    // 현재 메모리에 있는 모든 page의 forward distance를 저장할 배열을 동적할당
    int *forward_dis = (int*)malloc(sizeof(int) * M);

    for(int i=0; i<M; i++) {
        forward_dis[i] = -1;  // -1은 distance가 무한대임을 의미
    }
    
    // 모든 page의 forward distance를 구함
    for(int i=0; i<M; i++) {
        int page_num = memory[i];
        for(int j=time+1; j<K; j++) {
            if( page_num == pg_ref[j] ) {
                forward_dis[i] = j;
                break;
            }
        }
    }
    
    int Max = forward_dis[0];
    int Max_page = 0;

    // Forward Distance가 가장 큰 페이지가 victim
    for(int i=1; i<M; i++) {
        // tie-breaking rule: 낮은 번호의 page를 선택
        if( forward_dis[i] == -1 ) {
            if( memory[Max_page] > memory[i] ) {
                Max = forward_dis[i];
                Max_page = i;
            }
        } else {
            if( Max != -1 && forward_dis[i] > Max ) {
                Max = forward_dis[i];
                Max_page = i;
            }
        }
    }

    free(forward_dis);   // 자원 반환
    return Max_page;
}

// FIFO algorithm 적용
// victim의 page number를 반환함
int FIFO_algo(void) {
    // timestamp1: page memory load time
    // load time이 가장 작은 page가 victim이 됨
    int Min = timestamp1[0];
    int Min_page = 0;
    for(int i=1; i<M; i++) {
        if( Min > timestamp1[i] ) {
            Min = timestamp1[i];
            Min_page = i;
        }
    }
    
    return Min_page;
}

// LRU algorithm 적용
// victim의 page number를 반환함
int LRU_algo(void) {
    // timestamp2: last page reference time
    // 참조한지 가장 오래된 page가 victim이 됨
    int Min = timestamp2[0];
    int Min_page = 0;
    for(int i=1; i<M; i++) {
        if( Min > timestamp2[i] ) {
            Min = timestamp2[i];
            Min_page = i;
        }
    }
    
    return Min_page;
}

// LFU algorithm 적용
// victim의 page number를 반환함
int LFU_algo(void) {
    // reference_cnt: page reference count
    // 참조된 횟수가 가장 적은 page가 victim이 됨
    int page_num = memory[0];
    int Min = reference_cnt[page_num];
    int Min_page = 0;
    for(int i=1; i<M; i++) {
        page_num = memory[i];
        if( Min > reference_cnt[page_num] ) {
            Min = reference_cnt[page_num];
            Min_page = i;
        } else {
            if( Min == reference_cnt[page_num] ) {
                // tie-breaking rule : LRU
                // 참조된 횟수가 같다면 last page reference time을 확인함
                int first = timestamp2[Min_page];
                int second = timestamp2[i];
                if( first > second ) {
                    Min = reference_cnt[page_num];
                    Min_page = i;
                }
            }
        }
    }
    
    return Min_page;
}