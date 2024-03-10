#include <iostream> //para utilizar cin y cout
#include <stdlib.h> //para utilizar exit(), system()
#include <conio.h> //para utilizar getch()
#include <fstream> //para leer y escribir archivos
#include <sstream> //para convertir string a int
#include <cctype>// para utilizar isdigit()
#include <algorithm> // para utilizar all_of()
#include <limits> // Agregar esta línea para usar numeric_limits
using namespace std;

struct Cliente {
    int tarjeta;
    int pin;
    string nombre;
    int cbu;
    double saldo;
};

// Prototipos
void menuPrincipal();
bool buscarUsuario(int tarjeta, int& pin, Cliente& cliente);
bool verificarPin(int pin, int pinArchivo);
void mostrarSaldo(Cliente cliente);
void realizarDeposito(Cliente& cliente);
void realizarExtraccion(Cliente& cliente);
void realizarTransferencia(Cliente& cliente);
void actualizarArchivo(Cliente& cliente);
bool buscarDestinatarioPorCBU(int cbuIngresado, Cliente& destinatario);
bool comprobarDatoValido(int numero);

int main()
{
    //Declaramos las variables
    Cliente cliente;
    int opcion;
    char continuar;
    int contador = 0;
    int pinCorrecto; // Variable para almacenar el pin que se encuentra en el archivo
    bool opcionInvalida = true;
    do
    {
        cout << "Ingrese los ultimos 4 digitos de su tarjeta: ";
        string tarjetaIngresada;
        cin >> tarjetaIngresada;

        if (!all_of(tarjetaIngresada.begin(), tarjetaIngresada.end(), ::isdigit))
        {
            cout << endl
                << "Error: Los datos ingresados no son validos. Solo se aceptan numeros enteros positivos."
                << endl
                << endl
                << "Presione cualquier tecla para intentar nuevamente...";
            _getch();
            system("cls");
            continue;
        }

        cliente.tarjeta = stoi(tarjetaIngresada);

        if (!buscarUsuario(cliente.tarjeta, pinCorrecto, cliente))
        {
            cout << endl
                << "Numero de tarjeta no existe. Vuelva a intentar."
                << endl
                << endl
                << "Presione cualquier tecla para intentar nuevamente...";
            _getch();
            system("cls");
            continue;
        }
        system("cls");
    } while (!buscarUsuario(cliente.tarjeta, pinCorrecto, cliente));

    do
    {
        cout << "Ingrese su PIN: ";
        string pinIngresado;
        cin >> pinIngresado;

        if (!all_of(pinIngresado.begin(), pinIngresado.end(), ::isdigit))
        {
            cout << endl
                << "Error: Solo se aceptan valores numericos."
                << endl
                << endl
                << "Presione cualquier tecla para intentar nuevamente...";
            _getch();
            system("cls");
            continue;
        }

        cliente.pin = stoi(pinIngresado);

        if (!verificarPin(cliente.pin, pinCorrecto))
        {
            contador++;

            if (contador >= 3)
            {
                cout << endl
                    << "Su tarjeta ha sido retenida."
                    << endl;
                exit(1);
            }
            cout << endl
                << "PIN incorrecto."
                << endl
                << endl
                << "Presione cualquier tecla para intentar nuevamente...";
            _getch();
            system("cls");
            continue;
        }
        system("cls");
    } while (!verificarPin(cliente.pin, pinCorrecto));

    do
    {
        do
        {
            system("cls");
            cout << "Bienvenido, " << cliente.nombre << "!" << endl;
            menuPrincipal();
            cin >> opcion;
            
            if (cin.fail()) {
                cout << endl
                    << "Error: Solo se aceptan valores numericos."
                    << endl
                    << endl
                    << "Presione cualquier tecla para intentar nuevamente...";
                _getch();
                system("cls");
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                opcion = -1; // Asignamos -1 para que repita el bucle
                continue;
            } else if (opcion < 1 || opcion > 5)
            {
                cout << endl
                    << "Error: Solo se aceptan numeros entre 1 y 5."
                    << endl
                    << endl
                    << "Presione cualquier tecla para intentar nuevamente...";
                _getch();
                system("cls");
            }
        } while (opcion < 1 || opcion > 5);

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiamos el buffer de entrada

        switch (opcion)
        {
        case 1:
            mostrarSaldo(cliente);

            break;
        case 2:
            realizarDeposito(cliente);

            break;
        case 3:
            realizarExtraccion(cliente);

            break;
        case 4:
            realizarTransferencia(cliente);

            break;
        case 5:
            cout << "Muchas gracias por utilizar nuestros servicios…" << endl;
            exit(1);
            break;
        default:
            cout << endl
                << "Error: la opcion ingresada no es valida."
                << endl
                << endl
                << "Presione una tecla para intentar nuevamente...";
            _getch();
            break;
        }
        do 
        {
            cout << endl << "Desea realizar otra transaccion? (S/N): ";
            cin >> continuar;
            continuar = toupper(continuar); // Convertir a mayúscula (

            if (continuar != 'N' && continuar != 'S') {
                cout << "Error: la opcion ingresada no es valida." << endl;
                cin.clear(); // Limpiar el estado de error del cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorar el resto de la línea inválida
            }
        } while (continuar != 'N' && continuar != 'S');

    } while (continuar != 'N');

    return 0;
}

void menuPrincipal()
{
    cout << "  _________________________________" << endl
        << " |                                 |" << endl
        << " |        CAJERO AUTOMATICO        |" << endl
        << " |_________________________________|" << endl
        << " |                                 |" << endl
        << " |  1. Consultar saldo             |" << endl
        << " |  2. Deposito                    |" << endl
        << " |  3. Extraccion                  |" << endl
        << " |  4. Transferencia               |" << endl
        << " |  5. Cancelar transaccion        |" << endl
        << " |_________________________________|" << endl
        << endl
        << "Ingrese la opcion deseada: ";
}



bool buscarUsuario(int tarjetaIngresada, int& pin, Cliente& cliente)
{
    ifstream archivo;
    archivo.open("clientes.txt", ios::in);

    if (archivo.fail())
    {
        cout << "No se pudo abrir el archivo de clientes." << endl;
        exit(1);
    }

    string linea;
    char delimitador = ',';
    // Leemos todas las líneas
    while (getline(archivo, linea))
    {
        stringstream stream(linea); // Convertir la cadena a un stream
        string tarjetaCorrecta, pinCorrecto, cbu, nombre, saldo;

        getline(stream, tarjetaCorrecta, delimitador);
        int tarjeta = stoi(tarjetaCorrecta);

        if (tarjeta == tarjetaIngresada)
        {

            cliente.tarjeta = tarjeta;

            getline(stream, pinCorrecto, delimitador);
            istringstream(pinCorrecto) >> pin;

            getline(stream, cliente.nombre, delimitador);

            getline(stream, cbu, delimitador);
            istringstream(cbu) >> cliente.cbu;

            getline(stream, saldo);
            istringstream(saldo) >> cliente.saldo;

            archivo.close();
            return true;
        }
    }

    archivo.close();
    return false;
}

bool verificarPin(int pin, int pinArchivo)
{
    return (pin == pinArchivo); // Compara el pin ingresado con el pin correcto
}

void mostrarSaldo(Cliente cliente)
{
    cout << "Su saldo actual es: " << cliente.saldo << endl;
}

void realizarDeposito(Cliente& cliente) {
    int deposito;
    do
    {
        cout << "Ingrese el importe a depositar: ";
        cin >> deposito;
    } while (!comprobarDatoValido(deposito));
    cliente.saldo += deposito;
    actualizarArchivo(cliente);
    cout << "Deposito realizado correctamente. Saldo actual: " << cliente.saldo << endl;
}

void realizarExtraccion(Cliente& cliente) {
    int extraccion;

    do
    {
        cout << "Ingrese el importe que desea extraer: ";
        cin >> extraccion;
    } while (!comprobarDatoValido(extraccion));

    if (extraccion > cliente.saldo) {
        cout << "Error: Saldo insuficiente para realizar la extracción." << endl;
    }
    else {
        cliente.saldo -= extraccion;
        actualizarArchivo(cliente);
        cout << "Extraccion realizada correctamente. Saldo actual: " << cliente.saldo << endl;
    }
}

void realizarTransferencia(Cliente& cliente) {
    int cbu;
    double transferencia;
    do
    {
        cout << "Ingrese el CBU del destinatario: ";
        cin >> cbu;
    } while (!comprobarDatoValido(cbu));
    
    if (cbu == cliente.cbu) {
        cout << "Error: No se puede transferir al mismo CBU." << endl;
        return; // Salir de la función sin continuar con la transferencia
    }

    do
    {
        cout << "Ingrese el importe a transferir: ";
        cin >> transferencia;
    } while (!comprobarDatoValido(transferencia));

    if (transferencia > cliente.saldo) {
        cout << "Error: Saldo insuficiente para la transferencia." << endl;
    }
    else {
        Cliente destinatario;
        if (buscarDestinatarioPorCBU(cbu, destinatario)) {
            cout << "Destinatario: " << destinatario.nombre << endl;

            char confirmacion;
            cout << "Confirmar transferencia (S/N): ";
            cin >> confirmacion;
            if (confirmacion == 'S' || confirmacion == 's') {
                cliente.saldo -= transferencia;
                destinatario.saldo += transferencia;
                actualizarArchivo(cliente);
                actualizarArchivo(destinatario);
                cout << "Transferencia realizada correctamente." << endl;
            }
            else {
                cout << "Transferencia cancelada." << endl;
            }
        }
        else {
            cout << "Error: El destinatario no existe." << endl;
        }
    }
}

void actualizarArchivo(Cliente& cliente) {
    ifstream archivoIn;
    ofstream archivoOut;

    archivoIn.open("clientes.txt", ios::in);
    if (archivoIn.fail()) {
        cout << "No se pudo abrir el archivo de clientes." << endl;
        exit(1);
    }

    archivoOut.open("temp.txt", ios::out);
    if (archivoOut.fail()) {
        cout << "Error al crear archivo temporal." << endl;
        archivoIn.close();
        exit(1);
    }

    string linea;
    while (getline(archivoIn, linea)) {
        stringstream stream(linea);
        string dato;

        getline(stream, dato, ',');
        int tarjeta = stoi(dato);

        if (tarjeta == cliente.tarjeta) {
            archivoOut << tarjeta << "," << cliente.pin << "," << cliente.nombre << "," << cliente.cbu << "," << cliente.saldo << endl;
        }
        else {
            archivoOut << linea << endl;
        }
    }

    archivoIn.close();
    archivoOut.close();

    // Intercambio de archivos
    remove("clientes.txt");
    rename("temp.txt", "clientes.txt");
}

bool buscarDestinatarioPorCBU(int cbuIngresado, Cliente& destinatario)
{
    ifstream archivo;
    archivo.open("clientes.txt", ios::in);

    if (archivo.fail()) {
        cout << "No se pudo abrir el archivo de clientes." << endl;
        exit(1);
    }

    string linea;
    char delimitador = ',';
    // Leemos todas las líneas
    while (getline(archivo, linea))
    {
        stringstream stream(linea); // Convertir la cadena a un stream
        string tarjetaCorrecta, pinCorrecto, cbu, nombre, saldo;

        getline(stream, tarjetaCorrecta, delimitador);
        int tarjeta = stoi(tarjetaCorrecta);

        destinatario.tarjeta = tarjeta;

        getline(stream, pinCorrecto, delimitador);
        istringstream(pinCorrecto) >> destinatario.pin;

        getline(stream, destinatario.nombre, delimitador);

        getline(stream, cbu, delimitador);
        istringstream(cbu) >> destinatario.cbu;

        getline(stream, saldo);
        istringstream(saldo) >> destinatario.saldo;

        if (destinatario.cbu == cbuIngresado) {
            archivo.close();
            return true;
        }
    }

    archivo.close();
    return false;
}

bool comprobarDatoValido(int numero) {
    bool numeroValido = false;
    if (cin.fail()) {
        cout << "Error: Solo se aceptan valores numericos. Por favor intente nuevamente..." << endl;
        _getch();
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } else if (numero < 0) {
        cout << "El dato ingresado no es valido. Por favor intente nuevamente..." << endl;
        _getch();
    }
    else {
        numeroValido = true;
    }
    return numeroValido;
}