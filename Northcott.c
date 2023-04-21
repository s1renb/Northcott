#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

int field_row;                  // Длина поля
int field_col;                  // Ширина поля
int players_count;              // Количество игроков
int level;                      // Сложность
int gamer=1;                    // Текущий игрок (первоначально 1-ый)
char field[15][15];             // Поле размером 15 на 15
char name1[15], name2[15];      // Имена игроков по 15 символов
int pos1[15], pos2[15];         // Позиция шашек игроков по горизонтали
int moves_1, moves_2;           // Количество ходов, который совершил каждый игрок
int pos1_old[15], pos2_old[15]; // Старые позиции шашек игроков по горизонтали

/*  Объявление функций  */
void hello();
void rules_of_the_game();
void field_size();
void init_game();
void game_mode();
void print_state();
void print_path(int gamer, int row);
int change_pos(int gamer);
int change_pos_PC(int level);
int check_game_over();
FILE *open_save_file(int save_number, int mode);
void show_saves(int save_number);
int check_have_save(int save_number, int func);
int check_save_file(int save_number);
int save_game();
int loading_save_game();

/*  Функции: */
// Приветствие
void hello(void){
    printf("==================================================================\n");
    printf("\t\tДобро пожаловать в игру \"Норткотта\"\n");
    printf("==================================================================\n");
}
// Правила игры
void rules_of_the_game(void){
    char c;
    printf("Желаете ознакомиться с правилами игры? (Y/N)(y/n): ");
    while(1){
        scanf("%c", &c);
        if(c=='Y' || c=='y'){
            printf("============================================================================================================");
            printf("\nПравила игры \"Норткотта\":\n\n");
            printf("В начале игры в левом столбце во всех строках расположены черные шашки,\n");
            printf("в правом столбце - белые шашки.\n\n");
            printf("За один ход игрок может передвинуть любую свою шашку в одной строке на любое число полей в любую сторону.\n");
            printf("При этом нельзя перескакивать через шашки противника!\n\n");
            printf("Побеждает тот игрок, который своими шашками \"закрыл\" шашки противника.\n");
            system("Pause");
            printf("============================================================================================================\n");
            break;
        }
        else if(c=='N' || c=='n'){
            printf("==================================================================\n");
            break;
        }
    }
}
// Выбор размера поля
void field_size(void){
    printf("Максимальный размер поля: (15*15)\n");
    while(1){
        printf("Введите размер поля (M*N): ");
        scanf("%d*%d", &field_row, &field_col);
        if(field_row>15 || field_col>15 || field_col<3 || field_row<1)
            printf("Некорректный размер поля, повторите ввод\n");
        else{
            printf("==================================================================\n");
            break;
        }
    }
}
// Заполнение начальными данными
void init_game(void){
    int i;
    for(i=0;i<field_row;i++){
        field[i][0]=1;           // Белые шашки
        field[i][field_col-1]=2; // Черные шашки
        pos1[i]=0;               // Первоначальные позиции шашек 1-го игрока по горизонтали
        pos2[i]=field_col-1;     // Первоначальные позиции шашек 2-го игрока по горизонтали
        pos1_old[i]=0;           // Старые позиции шашек 1-го игрока по горизонтали
        pos2_old[i]=field_col-1; // Старые позиции шашек 2-го игрока по горизонтали
    }
    moves_1=0;                   // Количество ходов совершенным 1-ым игроком
    moves_2=0;                   // Количество ходов совершенным 2-ым игроком
    printf("==================================================================\n");
    printf("Идет загрузка...");
    sleep(1);
}
// Выбор режима игры (1 игрок/2 игрока), запись имен
void game_mode(void){
    printf("Сколько игроков будет играть? (1 или 2)\n");
    while(1){
        printf("Введите количество игроков: ");
        scanf("%d", &players_count);
        if(players_count==2){ // Если будут играть 2 игрока
            printf("Введите имя первого игрока: ");
            scanf("%s", name1);
            printf("Введите имя второго игрока: ");
            scanf("%s", name2);
            break;
        }
        else if(players_count==1){ // Если 1...
            printf("Введите имя игрока: ");
            scanf("%s", name1);
            strcpy(name2, "Компьютер");
            printf("Выберите сложность (1 - легко / 2 - средне / 3 - сложно): ");
            scanf("%d", &level);
            break;
        }
        else{
            printf("Некорректное число игроков. Повторите ввод.\n\n");
        }
    }
}
// Печать игрового поля
void print_state(void){
    int i,j,k;
    printf("\n%s (Слева)   vs   %s (Справа)\n\n", name1, name2); // Печать имен
    printf("     | ");
    for(i=0;i<field_col;i++)
        printf("%2d  | ", i+1); // Печать координат по горизонтали
    printf("\n");
    printf("-----|");
    for(i=0;i<field_col;i++)
        printf("-----|");
    printf("\n");
    for(i=0; i<field_row; i++){
        printf("%4d ", i+1); // Печать координат по вертикали
        for(j=0; j<field_col; j++)
            if(field[i][j]==0){
                printf("|  .  ");  // Печать пустых ячеек
            }
            else{
                field[i][j]==1 ? printf("|  O  ") : printf("|  X  "); // Печать шашек
            }
        printf("|");
        printf("\n");
        for(k=0;k<=field_col;k++)
            printf("-----|");
        printf("\n");
    }
}
// Печать пути прошлого хода
void print_path(int gamer, int row){
    if(gamer==1){ // Если ход совершает 1-ый игрок, то выводится информация о 2-ом
        printf("\nПредыдущий ход игрока №2 - %s: Ряд №%d (%d ---> %d)\n", name2, row, (pos2_old[row-1]+1), (pos2[row-1]+1));
        printf("-------------------------------------------------------");
    }
    else{ // Если 2-ой...
        printf("\nПредыдущий ход игрока №1 - %s: Ряд №%d (%d ---> %d)\n", name1, row, (pos1_old[row-1]+1), (pos1[row-1]+1));
        printf("-------------------------------------------------------");
    }
}
// Сдвиг шашек и проверка (для пользователей)
int change_pos(int gamer){
    int row, col;
    while(1){
        if(gamer==1){
            printf("\n%s, ", name1);
        }
        else{
            printf("\n%s, ", name2);
        }
        printf("введите позицию, в которую хотите поставить шашку (0 0 - выйти из игры): ");
        scanf("%d %d", &row, &col);
        if(row==0 && col==0){ // Если пользователь хочет выйти
            printf("=====================================================================================\n");
            return 0;
        }
        row=row-1;
        col=col-1;
        if(row>=0 && row<field_row && col>=0 && col<field_col) // Проверка диапазона
            if(field[row][col]==0 && gamer==1 && col<pos2[row]){ // Если ячейка не занята, игрок №1, и не переходит за другую шашку
                field[row][pos1[row]]=0; // Убираем шашку со старой позиции
                field[row][col]=1; // Ставим шашку в новую позицию
                pos1_old[row]=pos1[row];
                pos1[row]=col; // Фиксируем новое положение по горизонтали
                system("cls");
                break;
            }
            else if(field[row][col]==0 && gamer==2 && pos1[row]<col){ // Если ячейка не занята, игрок №2, и не переходит за другую шашку
                field[row][pos2[row]]=0; // Убираем шашку со старой ячейки
                field[row][col]=2; // Ставим шашку в новую ячейку
                pos2_old[row]=pos2[row]; // Записываем старое положение
                pos2[row]=col; // Записываем новое положение
                system("cls");
                break;
            }
            else
                printf("Некорректная позиция.");
    }
    if(gamer==1){ // Если ход совершил 1-ый игрок, то увеличиваем кол-во ходов 1-го игрока
        moves_1++;
    }
    else{ // Если 2-ой...
        moves_2++;
    }
    return (row+1);
}
// Сдвиг шашек и проверка (для компьютера)
int change_pos_PC(int level){
    int row, col, i, count=0, try=0;
    for(i=0;i<field_row;i++){
        if(pos2[i]==pos1[i]+1){
            count++; // Количество шашек, которые не могут ходить вперед
        }
    }
    srand(time(NULL));
    while(1){
        row=0+rand()%((field_row-1)-0+1);
        if(level==1){ // Легко
            col=1+rand()%((field_col-1)-1+1);
        }
        else if(level==2){ // Средне
            col=(pos1[row]+1)+rand()%((pos1[row]+3)-(pos1[row]+1)+1);
            if(count==field_row){
                col=(pos2[row]+1)+rand()%((pos2[row]+3)-(pos2[row]+1)+1);
            }
        }
        else if(level==3){ // Сложно
            col=pos1[row]+1;
            if(count==(field_row-3)){
                col=pos1[row]+1;
            }
            if(count==(field_row-2)){
                col=(pos1[row]+2)+rand()%((pos1[row]+4)-(pos1[row]+2)+1);
                if(try==(field_row+25)){
                    col=pos2[row]-1;
                    try=0;
                }
                if(col>=field_col){
                    try++;
                }
                if(col==pos2[row]){
                    col=pos2[row]+1;
                }
            }
            else if(count==(field_row-1)){
                col=pos1[row]+1;
            }
            else if(count==field_row){
                col=(pos2[row]+1)+rand()%((pos2[row]+3)-(pos2[row]+1)+1);
            }
        }
        if(row>=0 && row<field_row && col>=0 && col<field_col){ // Проверка диапазона
            if(field[row][col]==0 && pos1[row]<col){ // Если ячейка свободна и не "переходит" за другую шашку
                field[row][pos2[row]]=0; // Убираем шашку со старой ячейки
                field[row][col]=2; // Ставим шашку в новую ячейку
                pos2_old[row]=pos2[row]; // Записываем старое положение
                pos2[row]=col; // Записываем новое положение
                moves_2++; // Увеличиваем ходы компьютера
                sleep(1);
                system("cls");
                break;
            }
        }
    }
    return (row+1);
}
/* 
    Проверка завершения игры
    0 - Победу никто не одержал
    1 - Победу одержал один из игроков
*/
int check_game_over(void){
    int i, count_1=0, count_2=0;
    for(i=0;i<field_row;i++){
        if(field[i][field_col-2]==1){ // Если шашка первого игрока "перекрыла" шашку второго игрока
            count_1++;
        }
        else if(field[i][1]==2){ // Если шашка второго игрока "перекрыла" шашку первого игрока
            count_2++;
        }
    }
    if(count_1==field_row || count_2==field_row){
        printf("\nИгра окончена!\n");
        printf("==================================================================\n");
        if(count_1==field_row){
            printf("За %d ходов победу одержал %s.\n",moves_1, name1);
        }
        else{
            printf("За %d ходов победу одержал %s.\n",moves_2, name2);
        }
        return 1;
    }
    return 0;
}
/*  
    Открывает файл сохранения для определенной задачи:
    0 - Файл не открылся
    f - Файл открылся
    mode (1 - чтение, 2 - запись)
*/
FILE *open_save_file(int save_number, int mode){
    FILE *f;
    char namefile[10]={"save0.txt"};
    namefile[4]+=save_number;
    if(mode==1){
        f=fopen(namefile, "r");
    }
    else{
        f=fopen(namefile, "w");
    }
    if(!f){ // Если не открылся, то возвращаем 0
        return NULL;
    }
    else{ // Если открылся, то возвращаем указатель на файл
        return f;
    }
}
// Отображение слотов
void show_saves(int save_number){
    FILE* f;
    f=open_save_file(save_number, 1);
    if(f){ // Если файл открылся, то он существует
        printf("Слот №%d - Занят\n", save_number);
        fclose(f);
    }
    else{ // Файл не существует
        printf("Слот №%d - Свободен\n", save_number);
    }
}
/*  
    Проверка наличия сохранения:
    0 - Файл уже имеет сохранение и его НЕ перезаписывают
    1 - Файл уже имеет сохранение и его перезаписывают
    2 - Сохранения не существует
    3 - Слот для сохранения свободен
    func (1 - сохранение, 2 - загрузка)
*/
int check_have_save(int save_number, int func){
    char symbol;
    FILE* f;
    f=open_save_file(save_number, 1);
    if(f && func==1){ // Если файл открылся и его хотят сохранить
        printf("В данном слоте уже имеется сохранение.\nВы хотите перезаписать сохранение? (Y/N)(y/n): ");
        while(1){
            scanf("%c", &symbol);
            if(symbol=='Y' || symbol=='y'){
                return 1;
            }
            else if(symbol=='N' || symbol=='n'){
                return 0;
            }
        }
    }
    else if(!f && func==1){ // Если файл не открылся и его хотят сохранить, то слот свободен
        return 3;
    }
    else if(!f && func==2){ // Если файл не открылся и его хотят загрузить, то его не существ.
        printf("Сохранения №%d не существует! Выберите другой слот.\n", save_number);
        return 2;
    }
}
/*
    Проверка файла на целостность:
    0 - Файл поврежден
    1 - Файл целостен
*/
int check_save_file(int save_number){
    char str[15];
    int line=0;
    FILE* f;
    f=open_save_file(save_number, 1);
    fscanf(f,"%d ", &field_row);
    while(fgets(str,15,f)){ // Пока не достигнут конец файла считываем строки
        line++;
    }
    if(line-1!=(2+(4*field_row))){ // Если количество строк, которое должно быть, не совпадает с текущим
        printf("Файл сохранения №%d поврежден. Удалите и выберите другой.\n", save_number);
        field_row=0;
        return 0;
    }
    else{
        return 1;
    }
}
/*
    Сохранение текущей игры:
    0 - Выход из игры без сохранения
    1 - Выход из игры с сохранением
*/
int save_game(void){
    char symbol;
    int save_number, i, j, step;
    printf("Желаете сохранить игру? (Y/N)(y/n): ");
    while(1){
        scanf("%c", &symbol);
        if(symbol=='N' || symbol=='n'){
            printf("=====================================================================================\n");
            return 0;
        }
        else if(symbol=='Y' || symbol=='y'){
            break;
        }
    }
    while(1){
        printf("Выберите слот (1 - 3) (9 - выйти / 0 - просмотреть сохранения): ");
        scanf("%d", &save_number);
        if(save_number==9){
            printf("=====================================================================================\n");
            return 0;
        }
        if(save_number==0){
            for(i=1;i<=3;i++){
                show_saves(i); // Просмотр имеющихся сохранений
            }
            continue;
        }
        FILE* f;
        step=check_have_save(save_number, 1); // Проверка файла на наличие в нем уже сохранения
        if(!step){ // Если сохранение не стали перезаписывать - выбираем другой слот
            continue;
        }
        f=open_save_file(save_number, 2);
        fprintf(f,"%d %d %d %d %d %d %d\n", field_row, field_col, players_count, gamer, level, moves_1, moves_2);
        for(i=0;i<field_row;i++){
            fprintf(f,"%d\n%d\n%d\n%d\n", pos1[i], pos1_old[i], pos2[i], pos2_old[i]);
        }
        fprintf(f,"%s %s", name1, name2);
        fclose(f);
        break;
    }
    printf("\nИдет сохранение...");
    sleep(1);
    printf("\nИгра сохранена.\n\n");
    printf("==================================================================\n");
    return 1;
}
/*
    Загрузка ранее сохранненной игры:
    0 - Начало новой игры
    1 - Продолжить сохраненную игру
*/
int loading_save_game(void){
    char symbol;
    int save_number, i, step;
    printf("Желаете загрузить ранее сохранненную игру? (Y/N)(y/n): ");
    while(1){
        scanf("%c", &symbol);
        if(symbol=='N' || symbol=='n'){
            printf("==================================================================\n");
            return 0;
        }
        else if(symbol=='Y' || symbol=='y'){
            break;
        }
    }
    while(1){
        printf("Выберите слот (1 - 3) (9 - начать новую / 0 - просмотреть имеющиеся): ");
        scanf("%d", &save_number);
        if(save_number==9){
            printf("==================================================================\n");
            return 0;
        }
        if(save_number==0){
            for(i=1;i<=3;i++){
                show_saves(i); // Просмотр имеющихся сохранений
            }
            continue;
        }
        FILE* f;
        step=check_have_save(save_number, 2); // Проверка на наличие сохранения
        if(step==2){ // Если сохранения не существует - выбираем другое
            continue;
        }
        step=check_save_file(save_number); // Проверка файла на целостность
        if(!step){ // Если поврежден - выбираем другой
            continue;
        }
        f=open_save_file(save_number, 1);
        fscanf(f,"%d %d %d %d %d %d %d\n", &field_row, &field_col, &players_count, &gamer, &level, &moves_1, &moves_2);
        for(i=0;i<field_row;i++){
            fscanf(f,"%d\n%d\n%d\n%d\n", &pos1[i], &pos1_old[i], &pos2[i], &pos2_old[i]);
            field[i][pos1[i]]=1;
            field[i][pos2[i]]=2;
        }
        fscanf(f,"%s %s", &name1, &name2);
        fclose(f);
        break;
    }
    printf("==================================================================\n");
    printf("Идет загрузка...");
    sleep(1);
    return 1;
}
void main(){
    int step, row;
    hello();
    rules_of_the_game();
    step=loading_save_game();
    if(step==0){ // Если начинаем новую игру
        field_size();
        game_mode();
        init_game();
    }
    system("cls");
    print_state();
    if(players_count==2){ // Цикл для двух игроков
        while(1){
            step=check_game_over();
            if(step){ // Выход из игры если кто-то победил
                break;
            }
            row=change_pos(gamer);
            if(!row){ // Выход из игры по инициативе пользователя
                save_game();
                break;
            }
            print_state();
            gamer=3-gamer; // Смена игрока
            print_path(gamer, row);
        }
    }
    else { // Цикл для игры с компьютером
        while(1){
            row=change_pos(gamer);
            if(!row){ // Выход из игры по инициативе пользователя
                save_game();
                break;
            }
            print_state();
            step=check_game_over();
            if(step){ // Выход из игры если кто-то победил
                break;
            }
            printf("\nКомпьютер думает...");
            sleep(1);
            row=change_pos_PC(level);
            print_state();
            print_path(gamer, row);
            step=check_game_over();
            if(step){ // Выход из игры если кто-то победил
                break;
            }
        }
    }
    system("pause");
}
