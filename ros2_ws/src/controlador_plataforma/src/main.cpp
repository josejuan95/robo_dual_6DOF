#include <ncurses.h>
#include <unistd.h>
#include <ctime>
#include <locale.h> //para que deje poner simbolos
#include <stdlib.h> // para system()
#include <cmath>

#include "rclcpp/rclcpp.hpp" //ROS2
#include "geometry_msgs/msg/twist.hpp" //ROS2. plataforma móvil
#include "sensor_msgs/msg/joint_state.hpp" //ROS2. brazos

#define DEG2RAD(x) ((x)*M_PI/180.0)

int main(int argc, char **argv) {
    setlocale(LC_ALL,""); //habilita unicode

    // ROS2 setup
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("controlador_plataforma");
    auto pub_cmd = node->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    auto pub_AL = node->create_publisher<sensor_msgs::msg::JointState>("/AL/joint_states",10);
    auto pub_AR = node->create_publisher<sensor_msgs::msg::JointState>("/AR/joint_states",10);


    //Declaracion de variables que se usarán.
    // Variables de estado y copia previa para detectar cambios

    int al[6] = {0}, al_prev[6] = {0};
    int ar[6] = {0}, ar_prev[6] = {0};
    int avance = 0, avance_prev = 0;
    int turn = 0, turn_prev = 0;
    bool brazo_izquierdo = true; //para seleccionar que brazo usamos
    //Inicializar ncurses
    initscr();
    //Condiciones iniciales de como será la interfaz
    cbreak(); //no esperar al enter.
    noecho(); //no escribe lo que tecleo.
    keypad(stdscr, TRUE); //habilita poder usar las flechas del teclado.
    curs_set(0); //oculta el course.
    nodelay(stdscr,TRUE); //no bloquea el programa mientras no se pulse ninguna tecla.


    //Colores que se van a usar
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    init_pair(6, COLOR_BLACK, COLOR_BLACK);
    //Despliegue en la ventana
    while (rclcpp::ok()){

        //TÍTULO
        attron(COLOR_PAIR(1));
	mvprintw(1, 35,"CONTROL DE PLATAFORMA");


        //BRAZO IZQUIERDO
        mvprintw(4, 17,"BRAZO IZQUIERDO (1)");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
        mvprintw(6, 1 ,"VALOR");
	mvprintw(8, 1 ,"JOINT");
        mvprintw(12,1,"CTRL");
	attroff(COLOR_PAIR(2));
        mvprintw(6, 7,"[%3d"      ,al[0]); //valor en grados de los joints izquierdos
        mvprintw(6, 11,"] [%3d",al[1]);
        mvprintw(6, 17,"] [%3d",al[2]);
        mvprintw(6, 23,"] [%3d",al[3]);
        mvprintw(6, 29,"] [%3d",al[4]);
        mvprintw(6, 35,"] [%3d",al[5]);
        mvprintw(6, 41,"]");
	mvprintw(8, 8, "[1]   [2]   [3]   [4]   [5]   [6]");
	if (brazo_izquierdo==true){
	attron(COLOR_PAIR(5));
	mvprintw(11,9,"  Q     W     E     R     T     Y");
	attroff(COLOR_PAIR(5));
	attron(COLOR_PAIR(3));
	mvprintw(11,9,"+");mvprintw(11,15,"+");mvprintw(11,21,"+");mvprintw(11,27,"+");mvprintw(11,33,"+");mvprintw(11,39,"+");
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(5));
	mvprintw(13,9,"  A     S     D     F     G     H");
	attroff(COLOR_PAIR(5));
	attron(COLOR_PAIR(4));
        mvprintw(13,9,"-");mvprintw(13,15,"-");mvprintw(13,21,"-");mvprintw(13,27,"-");mvprintw(13,33,"-");mvprintw(13,39,"-");
	attroff(COLOR_PAIR(4));
	mvprintw(11,51,"+     +     +     +     +     +  ");
	mvprintw(13,51,"-     -     -     -     -     -  ");

    }
    else{
	 //BRAZO DERECHO
	attron(COLOR_PAIR(5));
        mvprintw(11,51,"  Q     W     E     R     T     Y");
        attroff(COLOR_PAIR(5));
        attron(COLOR_PAIR(3));
        mvprintw(11,51,"+");mvprintw(11,57,"+");mvprintw(11,63,"+");mvprintw(11,69,"+");mvprintw(11,75,"+");mvprintw(11,81,"+");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(5));
        mvprintw(13,51,"  A     S     D     F     G     H");
        attroff(COLOR_PAIR(5));
        attron(COLOR_PAIR(4));
        mvprintw(13,51,"-");mvprintw(13,57,"-");mvprintw(13,63,"-");mvprintw(13,69,"-");mvprintw(13,75,"-");mvprintw(13,81,"-");
        attroff(COLOR_PAIR(4));
        mvprintw(11,9,"+     +     +     +     +     +  ");
        mvprintw(13,9,"-     -     -     -     -     -  ");

    }

	attron(COLOR_PAIR(1));
        mvprintw(4, 60,"BRAZO DERECHO (2) ");
	attroff(COLOR_PAIR(1));
        mvprintw(6, 49,"[%3d"  ,ar[0]); //valor en grados de los joints izquierdos
        mvprintw(6, 53,"] [%3d",ar[1]);
        mvprintw(6, 59,"] [%3d",ar[2]);
        mvprintw(6, 65,"] [%3d",ar[3]);
        mvprintw(6, 71,"] [%3d",ar[4]);
        mvprintw(6, 77,"] [%3d",ar[5]);
        mvprintw(6, 83,"]");
        mvprintw(8, 50, "[1]   [2]   [3]   [4]   [5]   [6]");


        //MOVIMIENTO TRANSLACIONAL
	attron(COLOR_PAIR(1));
        mvprintw(16,43,"MOVIMIENTO");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
        mvprintw(16,60,"Velocidad: %3d",avance);
        mvprintw(17,60,"Giro: %3d",turn);
	attroff(COLOR_PAIR(2));
        mvprintw(19,47,"|");
        mvprintw(21,36,"<-----     |      ----->");
        move(18,47);addch(ACS_UARROW);
        move(22,47);addch(ACS_DARROW);
        refresh();

	//Explicación.
	mvprintw(24,2, "Use las flechas para avanzar/retroceder y realizar giros en la plataforma.");
	mvprintw(25,2, "Use la barra espaciadora para detener la plataforma.");
	mvprintw(26,2, "Use (1) y (2) para cambiar entre el brazo izquierdo y el brazo derecho.");
	mvprintw(27,2, "Use Q/W/E/R/T/Y para aumentar los grados de las uniones de los brazos.");
	mvprintw(28,2, "Use A/S/D/F/G/H para disminuir los grados de las uniones de los brazos.");
        int ch = getch();
        if (ch == ERR) {
            usleep(100000);
            continue;
        }

        switch (ch) {

            case '1': brazo_izquierdo=true; break;

            case '2': brazo_izquierdo=false; break;
//posicion de operacion
	    case '3': al[0]=0;al[1]=60;al[2]=45;al[3]=-60;al[4]=-90;ar[0]=0;ar[1]=-60;ar[2]=-45;ar[3]=60;ar[4]=90;break;
//posicion de descanso
            case '4': al[0]=7;al[1]=-97;al[2]=154;al[3]=-99;al[4]=90;al[5]=90;ar[0]=-7;ar[1]=97;ar[2]=-154;ar[3]=99;ar[4]=-90;ar[5]=-90;break;
//posicion de exposición
	    case '0': al[0]=0;al[1]=0;al[2]=0;al[3]=0;al[4]=0;al[5]=0;ar[0]=0;ar[1]=0;ar[2]=0;ar[3]=0;ar[4]=0;ar[5]=0; break;

	    case KEY_UP: avance++; break;

            case KEY_DOWN: avance--; break;

            case ' ': avance=0; turn=0; break;

            case KEY_RIGHT: turn--; break;

            case KEY_LEFT: turn++; break;

                case 'q': case 'Q':
                    if (brazo_izquierdo) al[0]++;
                    else ar[0]++;
                    break;
                case 'a': case 'A':
                    if (brazo_izquierdo) al[0]--;
                    else ar[0]--;
                    break;
                case 'w': case 'W':
                    if (brazo_izquierdo) al[1]++;
                    else ar[1]++;
                    break;
                case 's': case 'S':
                    if (brazo_izquierdo) al[1]--;
                    else ar[1]--;
                    break;
                case 'e': case 'E':
                    if (brazo_izquierdo) al[2]++;
                    else ar[2]++;
                    break;
                case 'd': case 'D':
                    if (brazo_izquierdo) al[2]--;
                    else ar[2]--;
                    break;
                case 'r': case 'R':
                    if (brazo_izquierdo) al[3]++;
                    else ar[3]++;
                    break;
                case 'f': case 'F':
                    if (brazo_izquierdo) al[3]--;
                    else ar[3]--;
                    break;
                case 't': case 'T':
                    if (brazo_izquierdo) al[4]++;
                    else ar[4]++;
                    break;
                case 'g': case 'G':
                    if (brazo_izquierdo) al[4]--;
                    else ar[4]--;
                    break;
                case 'y': case 'Y':
                    if (brazo_izquierdo) al[5]++;
                    else ar[5]++;
                    break;
                case 'h': case 'H':
                    if (brazo_izquierdo) al[5]--;
                    else ar[5]--;
                    break;
        }

	//Publicar cmd_vel si cambió avance o turn.

	if (avance != avance_prev || turn != turn_prev) {
            geometry_msgs::msg::Twist twist_msg;
            twist_msg.linear.x = static_cast<double>(avance);
            twist_msg.angular.z = static_cast<double>(turn);
            pub_cmd->publish(twist_msg);
            avance_prev = avance;
            turn_prev = turn;
        }
	//Publicar JointState de brazo izquierdo si cambia algún valor.

	bool cambio_AL = false;
        for (int i=0; i<6; i++) {
            if (al[i] != al_prev[i]) {
                cambio_AL = true;
                break;
            }
        }
        if (cambio_AL) {
            sensor_msgs::msg::JointState js_al;
            js_al.header.stamp = node->get_clock()->now();
            js_al.name = {"AL_joint_1", "AL_joint_2", "AL_joint_3", "AL_joint_4", "AL_joint_5", "AL_joint_6"};
            for (int i=0; i<6; i++)
                js_al.position.push_back(DEG2RAD(al[i]));
            pub_AL->publish(js_al);
            for (int i=0; i<6; i++) al_prev[i] = al[i];
        }


	// Publicar JointState de brazo derecho si cambió algún valor

        bool cambio_AR = false;
        for (int i=0; i<6; i++) {
            if (ar[i] != ar_prev[i]) {
                cambio_AR = true;
                break;
            }
        }
        if (cambio_AR) {
            sensor_msgs::msg::JointState js_ar;
            js_ar.header.stamp = node->get_clock()->now();
            js_ar.name = {"AR_joint_1", "AR_joint_2", "AR_joint_3", "AR_joint_4", "AR_joint_5", "AR_joint_6"};
            for (int i=0; i<6; i++)
                js_ar.position.push_back(DEG2RAD(ar[i]));
            pub_AR->publish(js_ar);
            for (int i=0; i<6; i++) ar_prev[i] = ar[i];
        }

        rclcpp::spin_some(node);
        
}
    endwin();
    rclcpp::shutdown();
    return 0;
}
