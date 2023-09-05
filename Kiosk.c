#include <stdio.h>
#include <pthread.h> // pthread
#include <stdlib.h> // system
#include <unistd.h> // sleep

/* 
    Kiosk Project Information
    base os : ubuntu-20.04.2.0  / compiler : gcc 9.3.0
    use ide : Visualstudio , goormide

    Patch note
    2021-04-18 : ++thread timer function
    2021-04-25 : ++kiosk menu, order system
    2021-04-26 : ++code simplify & code test, debug
                코드간소화
                 => 카테고리별 메뉴 배열을 전체 메뉴배열로 통합 ( 1차원 배열 -> 2차원 배열 )
                 => 메뉴 통합에 따른 가격 산정 알고리즘을 새로 작성하여 select_size -> select_size_price 로 변경
                    select_size_price 함수내에서 모든 메뉴의 가격 계산가능
                ++ 프로그램 종료시 최종 합산금액 표시
                ++ 메인메뉴에서는 카운터가 작동하지 않도록 수정

    Version information
    1.0 : 20초간 입력없을시 프로그램 종료 (싱글스레드 구조)
    2.0 : 20초간 입력없을시 키오스크 메인메뉴로 복귀 (멀티스레드 구조)
    2.1 : 글자 가시성 수정 및 메인 메뉴에서 입력전에 이동되지 않게 수정

    Debug list
    전반적인 코드중 선택지에 없는 숫자 입력시 재입력 받을수 있도록 수정
    쓰레드 작동시에 간헐적으로 임의로 숫자가 입력되는 오류 수정

	Project Function
	- f_keycheck() : scanf를 이용하여 사용자가 입력시 counter값이 1로 초기화하여 메인메뉴로 돌아가지 않게 적용
	- print_line() : 프롬프트 내에서 자주 출력되는 line 호출용
	- menu_kategorie(int order) : order가 1일때(주문희망) 카테고리 선택 함수 입력받은 카테고리 값을 int형으로 반환
	- order() : 고객의 주문희망 여부를 받는 함수 값을 int로 반환
	- select_menu(int menu_kategorie) : 주문받을 메뉴값을 받는 함수 메뉴가 있는 숫자만 입력 가능함
	- select_size_price(int kategorie, int select_menu) : 최종적으로 제품의 사이즈와 갯수를 입력받아서 금액을 반환함
	- show_menu(int kategorie) : 카테고리값에 따른 메뉴를 보여주는 함수
	Project Thread
	- f_counterzero(void *data) : 메인메뉴에서 timer 스레드가 작동하지 않도록 counter를 지속적으로 1로 유지시켜주는 함수
	                              메인메뉴에서 주문이 시작되면 종료된다.
	- f_kiosk(void *data) : 전반적인 Kiosk System이 실행되는 함수 실행직후 counterzero를 호출
	- f_timer(void *data) : 사용자의 입력이 없을시 메인메뉴로 전환되게끔 해주는 함수로서, 매초당 counter 값을 증가시키면서 확인한다.
	- main() : -
*/

// global variable
int counter = 1;
int price = 0;

int kategorie_size[8] = {11, 4, 9, 7, 7, 4, 7, 12};
int menu_price[8][30] = {
    { 4100, 4600, 4100, 4600, 4600, 5100, 4600, 5100, 5100, 5600, 5100, 5600, 5600, 6100, 5600, 6100, 600, 3300, 3800 },
    { 4800, 5300, 5300, 5800, 4500, 5000, 5000, 5500 },
    { 3500, 6300, 5500, 5500, 5500, 5500, 5500, 6000, 6000 },
    { 4800, 4800, 4800, 4800, 4800, 5800, 5800 },
    { 5500, 6000, 5500, 6000, 5300, 5800, 4800, 5300, 4800, 5300, 5500, 6000, 6000, 6500 },
    { 5500, 6000, 5500, 5500 },
    { 5300, 5800, 5300, 5800, 5800, 6300, 5800, 6300, 5200, 5700, 5200, 5700, 5200, 5700 },
    { 5900, 5700, 5900, 5000, 6500, 5800, 6200, 5500, 5700, 5000, 5800, 6300 }
}; // espresso , coldbrew , icecream , tea , tealatte , juiceade , iceblended , cake

int f_keycheck() {
    int output;                                           
    scanf("%d", &output);
    counter = 1;
    return output;
}

void print_line() {
	printf("-------------------------------------------------\n");
}

int menu_kategorie(int order) {
	int kategorie = 0;
	print_line();
	if (order == 1) {
		printf("1. 에스프레소 음료\n2. 콜드브루 음료\n3. 아이스크림/아포가토\n4. TWG TEA\n5. 티라떼\n6. 주스/에이드\n7. 아이스 블랜디드\n8. 케이크\n9. 취소\n");
		print_line();
		printf("메뉴 카테고리를 선택하세요. ");
        kategorie = f_keycheck();
		if(kategorie >= 1 && kategorie <= 9) {
			return kategorie;
		} else {
            printf("올바른 입력을 해주세요.\n");
			menu_kategorie(1);
		}
	}
	else if (order == 2) {
		printf("이용해주셔서 감사합니다.\n총 지불해야 할 금액은 %d 원 입니다.\n",price);
		exit(0);
	}
}

int order() {
	int order = 0;
	printf("주문하시겠습니까? (YES:1/NO:2) ");
    order = f_keycheck();
	do {
		if(order == 1 || order == 2) {
			return order;
			break;
		} else {
			print_line();
			printf("올바른 입력을 해주세요. ");
            order = f_keycheck();
		}
	} while (1);
	return order;
}

int select_menu(int menu_kategorie) {
	int menu_num = 0;
    do {
        print_line();
        printf("주문하실 메뉴를 선택해주세요. ");
        menu_num = f_keycheck();
        if(menu_num < 1 || menu_num > kategorie_size[menu_kategorie-1]) {
            printf("올바른 입력을 해주세요.\n");
        }
    } while(menu_num < 1 || menu_num > kategorie_size[menu_kategorie-1]);
	return menu_num;
}

int select_size_price(int kategorie, int select_menu) {
	int size = 0;
	int count = 0;
	int price = 0;
    if(kategorie==3 || kategorie == 4 || kategorie == 6 || kategorie == 8) {
        printf("해당 품목은 단일 사이즈 상품입니다.\n");
    } else {
        printf("사이즈를 선택해주세요. (Regular:1/Large:2) ");
        size = f_keycheck();
        while(size != 1 && size != 2) {
            printf("올바른 입력을 해주세요.\n");
            printf("사이즈를 선택해주세요. (Regular:1/Large:2) ");
            size = f_keycheck();
        }
    }
	printf("구입할 개수를 입력하세요. ");
    count = f_keycheck();
    if(kategorie==3 || kategorie == 4 || kategorie == 6 || kategorie == 8) {
        price = count * menu_price[kategorie-1][select_menu-1];
    } else {
	    price = count * menu_price[kategorie-1][2*select_menu-3+size];
    }
	printf("가격 : %d 원\n", price);
	return price;
}

int show_menu(int kategorie) {
	switch (kategorie) {
	case 1:
		print_line();
		printf("1. 롱블랙(Regular/Large) : 4100/4600원\n");
		printf("2. 아메리카노(Regular/Large) : 4100/4600원\n");
		printf("3. 숏라떼(Regular/Large) : 4600/5100원\n");
		printf("4. 카페라떼(Regular/Large) : 4600/5100원\n");
		printf("5. 카푸치노(Regular/Large) : 4600/5100원\n");
		printf("6. 바닐라 라떼(Regular/Large) : 5100/5600원\n");
		printf("7. 카페모카(Regular/Large) : 5100/5600원\n");
		printf("8. 카라멜 마끼아또(Regular/Large) : 5600/6100원\n");
		printf("9. 화이트 모카(Regular/Large) : 5600/6100원\n");
		printf("10. 플로팅 라떼(Regular) : 6000원\n");
		printf("11. 에스프레소(Regular/Large) : 3300/3800원\n");
		return 1;
		break;

	case 2:
		print_line();
		printf("1. 니트로 콜드브루(Regular/Large) : 4800/5300원\n");
		printf("2. 니트로 콜드브루 라떼(Regular/Large) : 5300/5800원\n");
		printf("3. 콜드브루(Regular/Large) : 4500/5000원\n");
		printf("4. 콜드브루 라떼(Regular/Large) : 5000/5500원\n");
		return 2;
		break;

	case 3:
		print_line();
		printf("1. 소프트 아이스크림 : 3500원\n");
		printf("2. 로얄 밀크티 쉐이크 : 6300원\n");
		printf("3. 바닐라 밀크쉐이크 : 5500원\n");
		printf("4. 초코 밀크쉐이크 : 5500원\n");
		printf("5. 커피 밀크쉐으크 : 5500원\n");
		printf("6. 아포가토 바닐라 : 5500원\n");
		printf("7. 아포가토 그린티 : 5500원\n");
		printf("8. 아포가토 블랙티 : 6000원\n");
		printf("9. 아포가토 아몬드 롤링 : 6000원\n");
		return 3;
		break;

	case 4:
		print_line();
		printf("1. 1387 블랙티(Large) : 4800원\n");
		printf("2. 프렌치 얼그레이(Large) : 4800원\n");
		printf("3. 잉글리쉬 블랙퍼스트(Large) : 4800원\n");
		printf("4. 크림 카라멜(Large) : 4800원\n");
		printf("5. 레드 자스민(Large) : 4800원\n");
		printf("6. 라즈베리 아이스티(Regular) : 5800원\n");
		printf("7. 허니 레몬 아이스티(Regular) : 5800원\n");
		return 4;
		break;

	case 5:
		print_line();
		printf("1. 로얄 밀크티(Regular/Large) : 5500/6000원\n");
		printf("2. 아이스 로얄 밀크티(Regular/Large) : 5500/6000원\n");
		printf("3. 마샬라 차이 라떼(Regular/Large) : 5300/5800원\n");
		printf("4. 그린티 라떼(Regular/Large) : 4800/5300원\n");
		printf("5. 아이스 그린티 라떼(Regular/Large) : 4800/5300원\n");
		printf("6. 버블 그린티 라떼(Regular/Large) : 5500/6000원\n");
		printf("7. 버블 밀크티(Regular/Large) : 6000/6500원\n");
		return 5;
		break;

	case 6:
		print_line();
		printf("1. - 키위 바나나 주스(Large) : 5500원\n");
		printf("2. - 오렌지 자몽 주스(Large) : 6000원\n");
		printf("3. - 라임 레몬 에이드(Large) : 5500원\n");
		printf("4. - 자몽 에이드(Large) : 5500원\n");
		return 6;
		break;

	case 7:
		print_line();
		printf("1. 커피 프라페(Regular/Large) : 5300/5800원\n");
		printf("2. 모카칩 카라멜(Regular/Large) : 5300/5800원\n");
		printf("3. 스트로베리 피치 프라페(Regular/Large) : 5800/6300원\n");
		printf("4. 요거 프라페(Regular/Large) : 5800/6300원\n");
		printf("5. 망고 프라페(Regular/Large) : 5200/5700원\n");
		printf("6. 유자 프라페(Regular/Large) : 5200/5700원\n");
		printf("7. 그린티 프라페(Regular/Large) : 5200/5700원\n");
		return 7;
		break;

	case 8:
		print_line();
		printf("1. 레드벨벳 : 5900원\n");
		printf("2. 트리플 초콜릿 무스 : 5700원\n");
		printf("3. 더치솔티드 카라멜 : 5900원\n");
		printf("4. 딸기 요거트 생크림 : 5000원\n");
		printf("5. 생크림 소프트 쉬폰 : 6500원\n");
		printf("6. 그뤼에르 치즈 무스 : 5800원\n");
		printf("7. 바닐라 크렘브륄레 : 6200원\n");
		printf("8. 클래식 갸또 : 5500원\n");
		printf("9. 뉴욕 치즈 케이크 : 5700원\n");
		printf("10. 티라미수 : 5000원\n");
		printf("11. 아이스박스 : 5800원\n");
		printf("12. 떠먹는 스트로베리 초콜릿 생크림 : 6300원\n");
		return 8;
		break;
	}
}

void *f_counterzero(void *data) { // 메인 메뉴인 동안 카운터가 작동되지 않도록하는 스레드
    while(1) {
        counter = 1;
        sleep(1);
    }
}

void *f_kiosk(void *data) { // 타이머 함수                                          
    int id;                             
    static int retval = 999;
    id = *((int *)data);
    
    int order_num = 0;
	int kategorie = 0;
	int moreorder = 1;
    price = 0;

    pthread_t c_thread;
    int cthr_id;
    int a = 1;
	cthr_id = pthread_create(&c_thread, NULL, f_counterzero, (void *)&a);
    if (cthr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

	print_line();
	printf("            ~~카페 키오스크 주문\n");
	print_line();
	
	order_num = order(); // 주문하기
    if(order_num == 1 || order_num == 2) {
        pthread_cancel(c_thread);
    }

	do {
		kategorie = menu_kategorie(order_num); // 카테고리 선택
		if(kategorie == 9) {
			print_line();
			printf("이용해주셔서 감사합니다.\n총 지불해야 할 금액은 %d 원 입니다.\n",price);
			exit(0);		
		}
		show_menu(kategorie); // 메뉴 표시, 카테고리 값 반환
		int se_menu = select_menu(kategorie); // 메뉴 선택
		price += select_size_price(kategorie, se_menu); // 가격 계산
		print_line();
		printf("총 지불해야 할 금액 : %d\n", price);
		print_line();
		do {
			printf("더 주문하시겠습니까? (YES:1/NO:2) ");
			moreorder = f_keycheck();
            if (moreorder == 2) {
				print_line();
				printf("이용해주셔서 감사합니다.\n총 지불해야 할 금액은 %d 원 입니다.\n",price);
				exit(0);
			} else if (moreorder == 1) {
				break;
			} else {
				print_line();
				printf("잘못 입력하였습니다.\n");	
			}
		} while (1);
	} while (moreorder == 1);
}

void *f_timer(void *data) { // 타이머 함수                                          
    int id;                             
    static int retval = 999;
    id = *((int *)data);
	
    int j=10;
    
    pthread_t k_thread;
    int kthr_id;
    int a = 1;
	kthr_id = pthread_create(&k_thread, NULL, f_kiosk, (void *)&a);
    if (kthr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

    do {
		if(counter == 1) {
			j=10;
		}
		if(counter == 10) {
			printf("\n입력이 없을시 10초후 초기화 됩니다.\n");
		} else if (counter > 10) {
			printf("Count = %d\n",j);
			j--;
		}
		counter++;
		sleep(1);
		if(counter==21) {
			//pthread_exit((void*)&retval); // 스레드 종료
            pthread_cancel(k_thread);
            kthr_id = pthread_create(&k_thread, NULL, f_kiosk, (void *)&a);
            if (kthr_id < 0)
            {
                perror("thread create error : ");
                exit(0);
            }
            counter = 1;
		}
    } while (1);
}

int main() {
	pthread_t p_thread; // timer thread
    int thr_id;
	void *tret = NULL;
    int a = 1;

	thr_id = pthread_create(&p_thread, NULL, f_timer, (void *)&a);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

	pthread_join(p_thread, &tret); // 쓰레드 Join
    /*if(*((int*)tret) == 999) { // 스레드 종료 확인 및 종료 코드
		pthread_cancel(p_thread[1]);
	}*/
	return 0;
}