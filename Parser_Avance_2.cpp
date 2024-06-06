#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

#define QF 100
#define QE -1

struct Token{
	string nombre;
	char tipo;
};

typedef struct Token TOKEN;

string parser(string cadenaFuente, int &j, TOKEN &tok);
void scanner(string cadenaFuente, int &j, TOKEN &tok);
void mostrarArchivo();
string reconocerDeclaraciones(string cadenaFuente, int &j, TOKEN &tok);
string reconocerOperaciones(string cadenaFuente, int &j, TOKEN &tok);

int main(){
	int j = 0;
	
	TOKEN tok;
	string linea, valido = "";
	
	tok.nombre = "";
	tok.tipo = '\0';
	
	mostrarArchivo();
	
	ifstream archivo("CadenaFuente2.cpp");
	
	if(!archivo.is_open()){
		cout << "Error al abrir el archivo." << endl;
	}
	
	cout << "PARSER: " << endl << endl;
	while(getline(archivo, linea) && valido != "ERROR"){
		j = 0;
		if(linea != ""){
			valido = parser(linea, j, tok);
			if (valido == "RECONOCE")
				cout << linea << "\t*RECONOCE*" << endl;
			else
				cout << linea << "\t*ERROR*" << endl;
		}
			
	}
	
	cout << endl;
	
	if(valido == "ERROR")
		cout << "CODIGO NO VALIDO";
	else
		cout << "CODIGO VALIDO";
	
	archivo.close();
	cout << endl;
	
	system("pause");
	return 0;
}

string parser(string cadenaFuente, int &j, TOKEN &tok){
	string expresion = "";
	
	//Regex para declaraciones 
	const regex expDec("^(entero|real|caracter|cadena)");

	//Regex para operaciones
	const regex expOper("^\\w+");
	
	if(regex_search(cadenaFuente, expDec)){
		expresion = reconocerDeclaraciones(cadenaFuente, j, tok);
	}
	else{
		if(regex_search(cadenaFuente,expOper)){
			expresion = reconocerOperaciones(cadenaFuente, j, tok);
		}
		else{
			return "ERROR";
		}
	}
	
	return expresion;	
		
	//cout << tok.nombre ;
	
}

void scanner(string cadenaFuente, int &j, TOKEN &tok){
	tok.nombre = "";
	tok.tipo = '\0';
	char c;
	
	while(cadenaFuente[j] == ' '){
		j++;
	}
	
	c = cadenaFuente[j];
	
	if(j >= cadenaFuente.length()){
		c = '$';
	}
	if(c >= 'a' && c <= 'z'){  //Letra
		while((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ){
			tok.nombre = tok.nombre + c;
			j++;
			c = cadenaFuente[j];
		}
		tok.tipo = 'I'; // Identificador
	}
	else if(c >= '0' && c <= '9'){ // Numero
		while(c >= '0' && c <= '9'){
			tok.nombre = tok.nombre + c;
			j++;
			c = cadenaFuente[j];
		}
		tok.tipo = 'N'; 
	}
	else if(c == ',' || c == '(' || c == ')' || c == '=' || c == '*' || c == '/'
			|| c == '-' || c == '+' || c == '<' || c == '>' || c == ';'){
				tok.nombre = c;
				if((c == '-' && cadenaFuente[j+1] == '-') ||
					(c == '+' && cadenaFuente[j+1] == '+') ||
					(c == '<' && cadenaFuente[j+1] == '<') ||
					(c == '>' && cadenaFuente[j+1] == '>') ||
					(c == '<' && cadenaFuente[j+1] == '=') ||
					(c == '>' && cadenaFuente[j+1] == '=') ||
					(c == '=' && cadenaFuente[j+1] == '=')){
						tok.nombre = tok.nombre + cadenaFuente[j+1];
						j++;
					}
					j++;
					tok.tipo = 'O'; //Operador
			}
	else if(c == 39 && isalpha(cadenaFuente[j+1]) && cadenaFuente[j+2] == 39){
		tok.nombre = tok.nombre + c;
		tok.nombre = tok.nombre + cadenaFuente[j+1];
		tok.nombre = tok.nombre + cadenaFuente[j+2];
		j = j + 3;
		tok.tipo = 'C'; // Caracter
	}
	
	else if(c == 34){
		tok.nombre = tok.nombre + c;
		j++;
		while(cadenaFuente[j] != 34){
			tok.nombre = tok.nombre + cadenaFuente[j];
			j++;
		}
		tok.nombre = tok.nombre + cadenaFuente[j];
		j++;
		tok.tipo = 'S'; // Cadena
	}
	
	else if(c == '$'){ // fin de cadena
		tok.nombre = c;
	}
	
	//cout << "  " << tok.nombre << endl;

}
void mostrarArchivo(){
	string linea;
	ifstream archivo("CadenaFuente2.cpp");
	
	if(!archivo.is_open()){
		cout << "Error al abrir el archivo." << endl;
	}
	
	cout << "CODIGO FUENTE: " << endl << endl;
	while(getline(archivo, linea)){
		cout << linea << endl;
	}
	cout << endl;
}

string reconocerDeclaraciones(string cadenaFuente, int &j, TOKEN &tok){
	int q = 0;

	while(q != QF && q != QE){
		scanner(cadenaFuente, j, tok);
				
		switch(q){
			case 0 :{
				if(tok.nombre == "entero" || tok.nombre == "real" || tok.nombre == "caracter"
					|| tok.nombre == "cadena"){
						q = 1;
					}
				else
					q = QE;
				break;
			}
			case 1 :{
				if(tok.tipo == 'I')
					q = 2;
				else
					q = QE;
				break;
			}
			case 2 :{
				if(tok.nombre == ",")
					q = 1;
				else if(tok.nombre == "=")
					q = 3;
				else if(tok.nombre == "$")
					q = QF;	// Estado final
				else
					q = QE;
					
				break;
			}
			case 3 :{
				if(tok.tipo == 'N' || tok.tipo == 'C' || tok.tipo == 'S')
					q = 2;
				else
					q = QE;
				break;
			}
		}
		
	}
	if(q == QF){
		return "RECONOCE";	
	}
	else{
		return "ERROR";
	}
		
	
}

string reconocerOperaciones(string cadenaFuente, int &j, TOKEN &tok){
	int q = 0;
	
	while (q != QF && q != QE) {
		
		scanner(cadenaFuente, j, tok);
	
        switch (q) {
            case 0:
                if (tok.tipo == 'I') {
                    q = 1;
                } else {
                    q = QE;
                }
                break;
            case 1:
                if (tok.nombre == "=") {
                    q = 2;
                } else {
                    q = QE;
                }
                break;
            case 2:
                if (tok.tipo == 'I' || tok.tipo == 'N') {
                    q = 3;
                } else {
                    q = QE;
                }
                break;
            case 3:
                if (tok.nombre == "$") {
                    q = QF;
                } else if (tok.tipo == 'O') {
                    q = 4;
                } else {
                    q = QE;
                }
                break;
            case 4:
                if (tok.tipo == 'I' || tok.tipo == 'N') {
                    q = 3;
                } else {
                    q = QE;
                }
                break;
        }
    }

    if (q == QF) {
        return "RECONOCE";
    } else {
        return "ERROR";
    }

}


