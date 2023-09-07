# Linux_Kiosk
C Language / Linux_Kiosk Program

# Kiosk Project Information
base os : ubuntu-20.04.2.0  / compiler : gcc 9.3.0 \n
use ide : Visualstudio , goormide

## Patch note
* 2021-04-18 : ++thread timer function
* 2021-04-25 : ++kiosk menu, order system
* 2021-04-26 : ++code simplify & code test, debug
 - 코드간소화
   + 카테고리별 메뉴 배열을 전체 메뉴배열로 통합 ( 1차원 배열 -> 2차원 배열 )
   + 메뉴 통합에 따른 가격 산정 알고리즘을 새로 작성하여 select_size -> select_size_price 로 변경 select_size_price 함수내에서 모든 메뉴의 가격 계산가능
 - ++ 프로그램 종료시 최종 합산금액 표시
 - ++ 메인메뉴에서는 카운터가 작동하지 않도록 수정

## Version information
- 1.0 : 20초간 입력없을시 프로그램 종료 (싱글스레드 구조)
- 2.0 : 20초간 입력없을시 키오스크 메인메뉴로 복귀 (멀티스레드 구조)
- 2.1 : 글자 가시성 수정 및 메인 메뉴에서 입력전에 이동되지 않게 수정

## Debug list
- 전반적인 코드중 선택지에 없는 숫자 입력시 재입력 받을수 있도록 수정
- 쓰레드 작동시에 간헐적으로 임의로 숫자가 입력되는 오류 수정

## Project Function
- f_keycheck() : scanf를 이용하여 사용자가 입력시 counter값이 1로 초기화하여 메인메뉴로 돌아가지 않게 적용
- print_line() : 프롬프트 내에서 자주 출력되는 line 호출용
- menu_kategorie(int order) : order가 1일때(주문희망) 카테고리 선택 함수 입력받은 카테고리 값을 int형으로 반환
- order() : 고객의 주문희망 여부를 받는 함수 값을 int로 반환
- select_menu(int menu_kategorie) : 주문받을 메뉴값을 받는 함수 메뉴가 있는 숫자만 입력 가능함
- select_size_price(int kategorie, int select_menu) : 최종적으로 제품의 사이즈와 갯수를 입력받아서 금액을 반환함
- show_menu(int kategorie) : 카테고리값에 따른 메뉴를 보여주는 함수

## Project Thread
- f_counterzero(void *data) : 메인메뉴에서 timer 스레드가 작동하지 않도록 counter를 지속적으로 1로 유지시켜주는 함수
                                메인메뉴에서 주문이 시작되면 종료된다.
- f_kiosk(void *data) : 전반적인 Kiosk System이 실행되는 함수 실행직후 counterzero를 호출
- f_timer(void *data) : 사용자의 입력이 없을시 메인메뉴로 전환되게끔 해주는 함수로서, 매초당 counter 값을 증가시키면서 확인한다.
- main() : -
