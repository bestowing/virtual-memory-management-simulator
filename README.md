### OS-project-2

# 가상 메모리 관리 시뮬레이터
*Virtual Memory Management Simulator*

본 문서는 '운영체제' 과목 수강중 배운 내용을 바탕으로, 가상 메모리 관리 기법을 구현한 시뮬레이터 구현 프로젝트를 설명합니다.

## 개요
본 프로그램은 표준 입력함수를 통해 console에서 process에 대한 정보와 몇 가지 제약 조건을 받아, Demand paging system에서 replacement 기법에 따른 시뮬레이션 결과를 다음과 같은 순서로 출력합니다.

* MIN, FIFO, LRU, LFU, WS 기법을 적용했을때,
    * 메모리 상태 변화 과정
    * page fault 발생 횟수 및 발생 위치

## 입출력 규칙
### 입력 규칙
본 프로그램은 정해진 format에 맞춘 데이터를 console에서 표준 입력 함수를 통해 읽습니다. 그 format은 다음과 같습니다.

| | | | | | | |
---|---|---|---|---|---|---|---
N | M | W | K
*r<sub>1</sub>* | *r<sub>2</sub>* | *r<sub>3</sub>* | *r<sub>4</sub>* | *r<sub>5</sub>* | ... | *r<sub>k</sub>*

* 첫째 행에서...
    * N은 process가 갖는 page 개수를 나타냅니다. (단, 5 <span>&#8804;</span> N <span>&#8804;</span> 15으로 가정하며, 범위에 대한 별도의 검사는 진행하지 않습니다)
    * M은 할당 page frame 개수를 나타냅니다. (고정 할당의 경우에만 사용합니다)
    * W는 window size를 나타냅니다. (WS 기법에서만 사용합니다)
    * K는 page reference string 길이를 나타냅니다.
* 둘째 행은 page reference string을 나타냅니다. (공백으로 구분됩니다)

예시는 아래와 같습니다:

| | | | | | | | | | | | | | |
---|---|---|---|---|---|---|---|---|---|---|---|---|---
6 | 4 | 3 | 14
1 | 2 | 6 | 1  | 4 | 5 | 1 | 2 | 1 | 4 | 5 | 6 | 4 | 5

### 출력 규칙
본 프로그램의 출력 예시는 아래와 같습니다.

![result1_1](https://user-images.githubusercontent.com/59321616/92320333-bce47380-f05b-11ea-8301-deb92091d181.png)
![result1_2](https://user-images.githubusercontent.com/59321616/92320334-be15a080-f05b-11ea-91d3-a4bb407e9199.png)
![result1_3](https://user-images.githubusercontent.com/59321616/92320335-be15a080-f05b-11ea-8cdc-88473efefd64.png)

* MIN, FIFO, LRU, LFU, WS 순서대로, 각 replacement 기법을 적용했을 때 메모리 상태의 변화 과정을 표 형식으로 한눈에 확인할 수 있도록 출력합니다.
* **[표]** 다음의 정보를 표시합니다: 시간, 접근할 page number, Page fault 발생 여부, 메모리 상태
    * 메모리 상태: page frame에 load된 page number를 출력합니다. page frame이 비어있다면, 공백으로 표시됩니다.
* 예외: WS 기법
    * 표에 **out** 칼럼이 추가됩니다: 해당 시각에 나가는 page의 번호를 출력합니다.
    * working set 개념을 고려, 메모리에 load된 page를 번호 오름차순으로 출력합니다.
* 메모리 상태 변화 과정을 출력한 뒤, 발생한 page fault의 수를 출력합니다.

## 세부사항
* 초기에 할당된 page frame은 모두 비어있는 상태로 가정합니다.
* MIN algorithm에서 tie-breaking rule은 항상 작은 page number를 기준으로 합니다.
* 어떤 page를 load할 때, 항상 가장 작은 번호의 frame에 load 합니다.
* 입력받은 정보 N, M, W, K에 대해 항상 아래의 조건을 만족합니다:
    * 5 <span>&#8804;</span> N <span>&#8804;</span> 15
    * 1 <span>&#8804;</span> M <span>&#8804;</span> N
    * W <span>&#8805;</span> 0
    * K <span>&#8805;</span> 1
* 즉, 사용자가 위의 조건을 만족하지 않는 정보를 입력할 경우, 프로그램의 정상적인 작동을 보장할 수 없습니다.

## 프로그램 설계

프로그램 구조에 대해 설명합니다. 파일이 기능별로 분할되어 있으며, 헤더파일을 포함해 총 5개 파일이 있습니다:
``main.h, main.c, setter.c, simulator.c, utility.c``

### 개요

| 소스 파일 | 함수 | 설명 |
|:---:|:---:|:---|
| main.h      | | 필요한 정적 라이브러리를 포함하고, 매크로 상수를 정의하는 헤더파일입니다. |
| main.c      | **main**         | 프로그램 실행 시작지점인 main 함수입니다.             |
| setter.c      | **set_simulation**         | 시뮬레이션을 위해 파일을 읽어오고 필요한 자원을 세팅하는 함수입니다.  |
| | init_ptr | 시뮬레이션을 위해 필요한 배열에 메모리를 동적할당하는 함수입니다. |
| | init_frame | page frame을 초기화하는 함수입니다. |
| simulator.c | **start_simulation** | 시뮬레이션의 핵심 과정을 실행하는 함수입니다. |
| | fixed_allo | 고정 할당기법을 구현한 함수입니다. |
| | variable_allo | 가변 할당기법을 구현한 함수입니다. |
| | MIN_algo | MIN algorithm을 구현한 함수입니다. |
| | FIFO_algo | FIFO algorithm을 구현한 함수입니다. |
| | LRU_algo | LRU algorithm을 구현한 함수입니다. |
| | LFU_algo | LFU algorithm을 구현한 함수입니다. |
| utility.c | print_table_Top | 표의 상단 부분을 출력하는 함수입니다. |
| |print_table_Content_fixed | 고정 할당기법의 시뮬레이션 결과를 출력하는 함수입니다. |
| | print_table_Content_vari | 가변 할당기법의 시뮬레이션 결과를 출력하는 함수입니다. |
| | print_table_Bottom | 표의 하단 부분을 출력하는 함수입니다. |
| | **resource_return** | 시뮬레이션을 위해 필요한 배열에 동적할당받은 메모리 bytes를 반환하는 함수입니다. |

### main.h

메인 헤더에 시뮬레이션에 필요한 정적 라이브러리와 매크로 함수가 명시되어있습니다.

~~~ c
#ifndef __MAIN_H
# define __MAIN_H

// 표준 입출력
#include <stdio.h>
// 메모리 동적 할당
#include <stdlib.h>

// boolean
#define true 1
#define false 0

// replacement 기법
#define MIN 0
#define FIFO 1
#define LRU 2
#define LFU 3
#define WS 4

#endif
~~~

### main.c

본 프로그램은 main 함수에서 다음과 같은 절차를 거쳐 실행됩니다.

1. 표준입력 함수를 통해 사용자로부터 입력을 받는다.
1. 시뮬레이션을 시작한다.
1. 사용한 메모리 자원을 반납하고 종료한다.

~~~ c
int main(int argc, char *argv[]) {
    if (set_simulation() == -1)    // 시뮬레이션 세팅
        return (0);
    start_simulation();  // 시뮬레이션 시작
    resource_return();   // 자원 반납
    return (0);
}
~~~

### setter.c

set_simulation() 함수에서 사용자의 입력을 받아, 배열에 메모리를 동적 할당하여 초기화합니다.
표준 입력과 메모리 동적 할당 과정에서 생길 수 있는 예외가 각 code number와 함께 처리되어 있습니다.

~~~ c
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
~~~

자세한 코드는 [레포지토리](https://github.com/bestowing/virtual-memory-management-simulator/blob/master/setter.c)를 참고하세요.

### simulator.c

start_simulation() 함수에서 시뮬레이션 핵심 과정이 진행됩니다.

* 고정 할당기법의 경우 fixed_allo() 함수를 호출합니다.
* 각 replacement 기법은 MIN_algo(),	FIFO_algo(), LRU_algo(), LFU_algo() 함수에 구현하여, victim이 될 page number를 반환합니다.
* fixed_allo() 함수의 작동 과정은 아래와 같습니다:
    1. 실행 전, 먼저 page frame을 초기화합니다.
    1. 현재 참조할 page를 page frame에서 찾습니다.
        1. frame에 존재하면, 해당 페이지를 참조하고 다음 page로 넘어갑니다.
    1. page fault가 발생했으면, 참조할 page를 빈 page frame에 로드합니다.
    1. 현재 page frame에 빈 곳이 없으면, 정해진 replacement 기법을 적용하여 찾은 victim 자리에 로드합니다.
* 가변 할당기법의 경우 variable_allo() 함수를 호출합니다.
    1. 실행 전, 먼저 page frame(working set)을 초기화합니다.
    1. 현재 시각 기준으로 working set을 구합니다.
    1. 현재 참조할 page를 working set에서 찾습니다.
        1. working set에 존재하면, 해당 페이지를 참조하고 다음 page로 넘어갑니다.
    1. page fault가 발생했으면, page frame을 추가로 할당받아 그곳에 로드합니다.
    1. working set에서 나가는 page가 있는지 확인합니다.

자세한 코드는 [레포지토리](https://github.com/bestowing/virtual-memory-management-simulator/blob/master/simulator.c)를 참고하세요.

### utility.c

동적할당한 메모리를 해제하거나, 시뮬레이션 결과를 출력하기 위한 함수가 있습니다.

자세한 코드는 [레포지토리](https://github.com/bestowing/virtual-memory-management-simulator/blob/master/utility.c)를 참고하세요.

## 테스트

다양한 입력을 통해 프로그램 실행 결과를 테스트합니다.

### 1. MIN, LFU algorithm의 tie-breaking rule을 보여주는 경우

입력은 아래와 같습니다:

 | |  |  |  | | | | | | | | |
---|---|---|---|---|---|---|---|---|---|---|---
5 | 3 | 2 | 12 | | | | | | | |
0 | 1 | 2 | 3 | 0 | 1 | 4 | 0 | 1 | 2 | 3 | 4

* 5개 페이지
* 3개 페이지 프레임
* window size = 2
* page reference string 길이 = 12
* page reference string = "012301401234"

출력은 아래와 같습니다:

![result2_1](https://user-images.githubusercontent.com/59321616/92320336-beae3700-f05b-11ea-82e6-31d0154e5f7a.png)
![result2_2](https://user-images.githubusercontent.com/59321616/92320337-bf46cd80-f05b-11ea-86c5-e6f425f32324.png)

* MIN algorithm을 적용한 경우, Time이 10이 될 때, page 0과 page 1 모두 forward distance가 무한대(∞)이므로, tie-breaking rule이 적용되어 page 0가 victim이 되었습니다. Time이 11이 될 때도 page 2와 page 1 에 tie-breaking rule이 적용되어 page 1이 victim이 되었습니다.
* LFU algorithm을 적용한 경우, Time 4에 page 0, 1, 2의 reference count가 모두 1로 
같으므로, tie-breaking rule인 LRU algorithm을 적용되어 마지막 참조 시간이 가장 긴 page 0이 victim이 되었습니다.

### 2. FIFO anomaly를 보여주는 케이스

입력은 아래와 같습니다:

 | |  |  |  | | | | | | | | |
---|---|---|---|---|---|---|---|---|---|---|---
5 | 4 | 2 | 12 | | | | | | | |
0 | 1 | 2 | 3 | 0 | 1 | 4 | 0 | 1 | 2 | 3 | 4

* 5개 페이지
* **4개** 페이지 프레임
* window size = 2
* page reference string 길이 = 12
* page reference string = "012301401234"
    * 케이스 1과 완전히 동일하나, page frame만 3에서 4로 추가로 하나가 더 할당된 경우입니다.
    * FIFO anomaly를 확인할 수 있습니다. *(memory frame이 추가로 할당되었으나, 오히려 page fault 발생 빈도가 증가하는 것)*

출력은 아래와 같습니다:

![result3_1](https://user-images.githubusercontent.com/59321616/92320338-bf46cd80-f05b-11ea-8558-f09ea22904b5.png)
![result3_2](https://user-images.githubusercontent.com/59321616/92320339-bfdf6400-f05b-11ea-80b6-1a18e3b02b3e.png)

* 케이스 1과 비교했을때, page fault 발생 횟수가 1번 증가하였습니다.

### 3. 정상적인 케이스

| | | | | | | | | | | | | | |
---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
6 | 4 | 3 | 14 | | | | | |
0 | 1 | 5 | 0 | 3 | 4 | 0 | 1 | 0 | 3 | 4 | 5 | 3 | 4

* 6개 페이지
* 4개 페이지 프레임
* window size = 3
* page reference string 길이 = 14
* page reference string = "0153401034534"

출력은 아래와 같습니다:

![result4_1](https://user-images.githubusercontent.com/59321616/92320340-bfdf6400-f05b-11ea-9915-3b1a85d18928.png)
![result4_2](https://user-images.githubusercontent.com/59321616/92320341-c077fa80-f05b-11ea-829e-7627ef7509f1.png)
![result4_3](https://user-images.githubusercontent.com/59321616/92320342-c1109100-f05b-11ea-8d8e-8b08a22e83ae.png)

## 에러 코드별 해결 방법

* **Error code 01**: console에서 입력받은 데이터를 표준 입력 함수를 통해 읽는데 실패했을때 발생합니다.

* **Error code 02**: 시뮬레이션에 필요한 배열 메모리 할당에 실패했을때 발생합니다. 실행 환경의 메모리 공간이 부족하지 않은지 확인하세요.