#include <iostream>
#include <string>
#include <sstream>    // Para stringstream
#include <vector>     // Para manejar los nibbles
#include <algorithm>  // Para std::reverse
#include <iomanip>    // Para setw y setfill

using namespace std;

// --- Herramientas de Conversión (Helpers) ---

/**
 * @brief Convierte un entero (0-15) a un string binario de 4 bits.
 */
string int_a_bin4(int n) {
    stringstream ss;
    // Asegura que el número esté en el rango de 4 bits (maneja desbordes)
    n = n & 0x0F; // n AND 1111
    for (int i = 3; i >= 0; --i) {
        ss << ((n >> i) & 1);
    }
    return ss.str();
}

/**
 * @brief Convierte un string binario a un entero.
 */
int bin_a_int(string bin) {
    if (bin.empty()) return 0;
    try {
        return stoi(bin, nullptr, 2);
    } catch (const invalid_argument& e) {
        cout << "Error: String binario invalido: " << bin << endl;
        return 0;
    }
}

/**
 * @brief Invierte todos los bits de un string binario (0->1, 1->0).
 */
string invertir_bits(string bin) {
    stringstream ss;
    for (char c : bin) {
        ss << (c == '0' ? '1' : '0');
    }
    return ss.str();
}

/**
 * @brief Convierte un número decimal (int) a su representación BCD (string).
 */
string decimal_a_bcd(int n) {
    string s_decimal = to_string(n);
    stringstream ss_bcd;
    for (char c : s_decimal) {
        int digito = c - '0';
        ss_bcd << int_a_bin4(digito);
    }
    return ss_bcd.str();
}

/**
 * @brief Convierte un string BCD a un entero decimal.
 */
int bcd_a_decimal(string bcd) {
    stringstream ss_decimal;
    for (size_t i = 0; i < bcd.length(); i += 4) {
        string grupo_bits = bcd.substr(i, 4);
        int val = bin_a_int(grupo_bits);
        ss_decimal << val;
    }
    if (ss_decimal.str().empty()) return 0;
    return stoi(ss_decimal.str());
}

/**
 * @brief Convierte un número decimal (int) a su representación en Exceso-3 (string).
 */
string decimal_a_exceso3(int n) {
    string s_decimal = to_string(n);
    stringstream ss_ex3;
    for (char c : s_decimal) {
        int digito = c - '0';
        ss_ex3 << int_a_bin4(digito + 3); // Lógica de Exceso-3
    }
    return ss_ex3.str();
}

/**
 * @brief Convierte un string Exceso-3 a un entero decimal.
 */
int exceso3_a_decimal(string ex3) {
    stringstream ss_decimal;
    for (size_t i = 0; i < ex3.length(); i += 4) {
        string grupo_bits = ex3.substr(i, 4);
        int val = bin_a_int(grupo_bits);
        ss_decimal << (val - 3); // Lógica inversa: resta 3
    }
    if (ss_decimal.str().empty()) return 0;
    return stoi(ss_decimal.str());
}

/**
 * @brief Añade relleno de 'dígitos' (grupos de 4 bits) al inicio para igualar longitudes.
 */
void pad_numeros(string& a, string& b, string pad_digit = "0000") {
    int len_a = a.length();
    int len_b = b.length();

    // 1. Asegurar que ambas longitudes sean múltiplos de 4
    if (len_a % 4 != 0) {
        a.insert(0, 4 - (len_a % 4), '0'); 
    }
    if (len_b % 4 != 0) {
        b.insert(0, 4 - (len_b % 4), '0');
    }

    len_a = a.length();
    len_b = b.length();

    // 2. Igualar longitudes usando el dígito específico
    if (len_a < len_b) {
        int digitos_a_anadir = (len_b - len_a) / 4;
        for (int i = 0; i < digitos_a_anadir; i++) {
            a.insert(0, pad_digit);
        }
    } else if (len_b < len_a) {
        int digitos_a_anadir = (len_a - len_b) / 4;
        for (int i = 0; i < digitos_a_anadir; i++) {
            b.insert(0, pad_digit);
        }
    }
}


// --- Lógica de Operaciones BCD ---

/**
 * @brief Realiza la suma BCD interna, dígito por dígito.
 */
string realizar_suma_bcd(string bcd_a, string bcd_b, int& carry, bool log_pasos) {
    
    string resultado_final_bcd = "";
    int num_grupos = bcd_a.length() / 4;

    for (int i = num_grupos - 1; i >= 0; --i) {
        string digito_a_str = bcd_a.substr(i * 4, 4);
        string digito_b_str = bcd_b.substr(i * 4, 4);

        int digito_a = bin_a_int(digito_a_str);
        int digito_b = bin_a_int(digito_b_str);

        if (log_pasos) {
            cout << "\nAnalizando Digito " << (num_grupos - i) << " (derecha a izquierda):" << endl;
            cout << "  " << digito_a_str << " (" << digito_a << ")" << endl;
            cout << "+ " << digito_b_str << " (" << digito_b << ")" << endl;
            cout << "+ " << setw(4) << setfill('0') << carry << " (Acarreo anterior)" << endl;
            cout << "  ----" << endl;
        }

        int suma_parcial = digito_a + digito_b + carry;
        
        int carry_out = 0;
        if (suma_parcial > 15) {
            carry_out = 1;
            suma_parcial -= 16;
        }

        if (log_pasos) {
            cout << "  = " << int_a_bin4(suma_parcial) << " (" << suma_parcial << ") [Acarreo binario: " << carry_out << "]" << endl;
        }

        bool necesita_correccion = (suma_parcial > 9) || (carry_out == 1);
        
        if (necesita_correccion) {
            if (log_pasos) {
                cout << "  -> RESULTADO INVALIDO (Suma > 9 o Acarreo binario=1)" << endl;
                cout << "  Aplicando correccion: Sumar 6 (0110)" << endl;
            }
            
            int suma_corregida = suma_parcial + 6;
            
            if (suma_corregida > 15) {
                carry = 1; 
                suma_corregida -= 16;
            } else {
                carry = 0;
            }
            
            if (carry_out == 1) carry = 1;

            if (log_pasos) {
                cout << "  = " << int_a_bin4(suma_corregida) << " [Nuevo Acarreo Decimal: " << carry << "]" << endl;
            }
            resultado_final_bcd.insert(0, int_a_bin4(suma_corregida));
        } else {
            if (log_pasos) {
                cout << "  -> RESULTADO VALIDO (<= 9 y Acarreo binario=0)" << endl;
            }
            carry = 0; 
            resultado_final_bcd.insert(0, int_a_bin4(suma_parcial));
        }
        if (log_pasos) cout << "------------------------------------------" << endl;
    }
    
    return resultado_final_bcd;
}

/**
 * @brief Calcula el complemento a 9 de un número BCD.
 */
string complemento_a9_bcd(string bcd) {
    stringstream ss_c9;
    for (size_t i = 0; i < bcd.length(); i += 4) {
        string digito_str = bcd.substr(i, 4);
        int digito_int = bin_a_int(digito_str);
        
        int c9_int = 9 - digito_int;
        ss_c9 << int_a_bin4(c9_int);
    }
    return ss_c9.str();
}


// --- FASE 1: Simulación de Suma BCD ---

void simular_suma_bcd() {
    int num1_int, num2_int;

    cout << "\n--- Fase 1: Sumador BCD ---" << endl;
    cout << "Introduce el primer numero (decimal): ";
    cin >> num1_int;
    cout << "Introduce el segundo numero (decimal): ";
    cin >> num2_int;

    string bcd_a = decimal_a_bcd(num1_int);
    string bcd_b = decimal_a_bcd(num2_int);

    // *** CORRECCIÓN ***
    // Añadir un dígito BCD '0' extra al frente para manejar el acarreo
    bcd_a.insert(0, "0000");
    bcd_b.insert(0, "0000");
    // Rellenar con el dígito '0' de BCD
    pad_numeros(bcd_a, bcd_b, "0000");

    cout << "\nPaso 1: Conversion y Relleno (con digito extra para acarreo)" << endl;
    cout << setw(8) << num1_int << " en BCD -> " << bcd_a << endl;
    cout << setw(8) << num2_int << " en BCD -> " << bcd_b << endl;
    cout << "------------------------------------------" << endl;

    int carry = 0;
    string resultado_final_bcd = realizar_suma_bcd(bcd_a, bcd_b, carry, true);

    // El acarreo final ya está incluido en el dígito extra, no se necesita 'if (carry == 1)'

    cout << "\n=== Resultado Suma BCD ===" << endl;
    cout << "Resultado en BCD: " << resultado_final_bcd << endl;
    
    int resultado_int = bcd_a_decimal(resultado_final_bcd);
    cout << "Resultado en Decimal: " << resultado_int << endl;
    
    if (resultado_int == num1_int + num2_int) {
        cout << "(Verificacion: CORRECTO)" << endl;
    } else {
        cout << "(Verificacion: INCORRECTO. Esperado: " << num1_int + num2_int << ")" << endl;
    }
}


// --- FASE 2: Simulación de Resta BCD 

void simular_resta_bcd() {
    int num1_int, num2_int;

    cout << "\n\n--- Fase 2: Restador BCD (Complemento a 10) ---" << endl;
    cout << "Introduce el minuendo (A): ";
    cin >> num1_int;
    cout << "Introduce el sustraendo (B): ";
    cin >> num2_int;

    string bcd_a = decimal_a_bcd(num1_int);
    string bcd_b = decimal_a_bcd(num2_int);

    // *** CORRECCIÓN 
    bcd_a.insert(0, "0000");
    bcd_b.insert(0, "0000");
    pad_numeros(bcd_a, bcd_b, "0000");


    cout << "\nPaso 1: Conversion y Relleno (con digito extra)" << endl;
    cout << "A: " << setw(8) << num1_int << " en BCD -> " << bcd_a << endl;
    cout << "B: " << setw(8) << num2_int << " en BCD -> " << bcd_b << endl;
    cout << "------------------------------------------" << endl;

    // A - B = A + C10(B) = A + C9(B) + 1
    
    cout << "Paso 2: Calcular Complemento a 9 del sustraendo (B)" << endl;
    string c9_b = complemento_a9_bcd(bcd_b);
    cout << "C9(" << num2_int << ") -> " << c9_b << " (" << bcd_a_decimal(c9_b) << ")" << endl;
    
    cout << "\nPaso 3: Sumar A + C9(B) + 1 (El +1 entra como acarreo inicial)" << endl;
    
    int carry_final = 1; // El "+1" del Complemento a 10
    string resultado_parcial = realizar_suma_bcd(bcd_a, c9_b, carry_final, true);

    cout << "\nPaso 4: Interpretar Acarreo Final" << endl;
    
    string resultado_final_bcd;
    int resultado_int;

    if (carry_final == 1) {
        cout << "  -> HUBO ACARREO FINAL (Carry = 1)." << endl;
        cout << "  -> El resultado es POSITIVO." << endl;
        cout << "  -> Se descarta el acarreo." << endl;
        
        resultado_final_bcd = resultado_parcial;
        resultado_int = bcd_a_decimal(resultado_final_bcd);

    } else {
        cout << "  -> NO HUBO ACARREO FINAL (Carry = 0)." << endl;
        cout << "  -> El resultado es NEGATIVO y esta en Complemento a 10." << endl;
        cout << "  -> Resultado parcial (en C10): " << resultado_parcial << endl;

        cout << "\nPaso 5: Re-complementar el resultado para verlo en decimal" << endl;
        
        string c9_res = complemento_a9_bcd(resultado_parcial);
        cout << "  C9 del resultado (" << resultado_parcial << ") -> " << c9_res << endl;

        cout << "  Sumando 1 para obtener C10 (magnitud)..." << endl;
        string uno_bcd(c9_res.length() - 4, '0');
        uno_bcd += "0001";
        
        int carry_recomp = 0; 
        resultado_final_bcd = realizar_suma_bcd(c9_res, uno_bcd, carry_recomp, false); 
        
        resultado_int = -bcd_a_decimal(resultado_final_bcd);
    }

    cout << "\n=== Resultado Resta BCD ===" << endl;
    cout << "Resultado en BCD (Magnitud): " << resultado_final_bcd << endl;
    cout << "Resultado en Decimal: " << resultado_int << endl;

    if (resultado_int == num1_int - num2_int) {
        cout << "(Verificacion: CORRECTO)" << endl;
    } else {
        cout << "(Verificacion: INCORRECTO. Esperado: " << num1_int - num2_int << ")" << endl;
    }
}


// --- FASE 3: Simulación de Suma Exceso-3

void simular_suma_exceso3() {
    int num1_int, num2_int;

    cout << "\n\n--- Fase 3: Sumador Exceso-3 ---" << endl;
    cout << "Introduce el primer numero (decimal): ";
    cin >> num1_int;
    cout << "Introduce el segundo numero (decimal): ";
    cin >> num2_int;

    string ex3_a = decimal_a_exceso3(num1_int);
    string ex3_b = decimal_a_exceso3(num2_int);

    // Añadir un dígito '0' (en Exceso-3) al frente para el acarreo
    ex3_a.insert(0, "0011");
    ex3_b.insert(0, "0011");
    // Rellenar con el dígito '0' de Exceso-3
    pad_numeros(ex3_a, ex3_b, "0011");


    cout << "\nPaso 1: Conversion a Exceso-3 y Relleno (con digito extra)" << endl;
    cout << setw(8) << num1_int << " en Exceso-3 -> " << ex3_a << endl;
    cout << setw(8) << num2_int << " en Exceso-3 -> " << ex3_b << endl;
    cout << "------------------------------------------" << endl;

    int carry = 0;
    string resultado_final_ex3 = "";
    int num_grupos = ex3_a.length() / 4;

    cout << "Paso 2: Suma binaria directa y Correccion (digito por digito)" << endl;
    
    for (int i = num_grupos - 1; i >= 0; --i) {
        string digito_a_str = ex3_a.substr(i * 4, 4);
        string digito_b_str = ex3_b.substr(i * 4, 4);

        int digito_a = bin_a_int(digito_a_str);
        int digito_b = bin_a_int(digito_b_str);

        cout << "\nAnalizando Digito " << (num_grupos - i) << " (derecha a izquierda):" << endl;
        cout << "  " << digito_a_str << " (" << digito_a << ")" << endl;
        cout << "+ " << digito_b_str << " (" << digito_b << ")" << endl;
        cout << "+ " << setw(4) << setfill('0') << carry << " (Acarreo anterior)" << endl;
        cout << "  ----" << endl;

        // 1. Suma binaria
        int suma_parcial = digito_a + digito_b + carry;

        if (suma_parcial > 15) {
            carry = 1;
            suma_parcial -= 16;
        } else {
            carry = 0;
        }

        cout << "  = " << int_a_bin4(suma_parcial) << " (" << suma_parcial << ") [Acarreo binario: " << carry << "]" << endl;

        // 2. Corrección Exceso-3
        int suma_corregida;
        if (carry == 1) {
            cout << "  -> HUBO ACARREO. Correccion: Sumar 3 (0011)" << endl;
            suma_corregida = suma_parcial + 3;
        } else {
            cout << "  -> NO HUBO ACARREO. Correccion: Restar 3 (0011)" << endl;
            suma_corregida = suma_parcial - 3;
        }
        
        resultado_final_ex3.insert(0, int_a_bin4(suma_corregida));
        cout << "  = " << int_a_bin4(suma_corregida) << " (Resultado Corregido del Digito)" << endl;
        cout << "------------------------------------------" << endl;
    }
    
    cout << "\n=== Resultado Suma Exceso-3 ===" << endl;
    cout << "Resultado en Exceso-3: " << resultado_final_ex3 << endl;

    int resultado_int = exceso3_a_decimal(resultado_final_ex3);
    cout << "Resultado en Decimal: " << resultado_int << endl;

    if (resultado_int == num1_int + num2_int) {
        cout << "(Verificacion: CORRECTO)" << endl;
    } else {
        cout << "(Verificacion: INCORRECTO. Esperado: " << num1_int + num2_int << ")" << endl;
    }
}


// --- FASE 4: Simulación de Resta Exceso-3 

void simular_resta_exceso3() {
    int num1_int, num2_int;

    cout << "\n\n--- Fase 4: Restador Exceso-3 (Complemento a 9) ---" << endl;
    cout << "Introduce el minuendo (A): ";
    cin >> num1_int;
    cout << "Introduce el sustraendo (B): ";
    cin >> num2_int;

    string ex3_a = decimal_a_exceso3(num1_int);
    string ex3_b = decimal_a_exceso3(num2_int);


    ex3_a.insert(0, "0011");
    ex3_b.insert(0, "0011");
    pad_numeros(ex3_a, ex3_b, "0011");


    cout << "\nPaso 1: Conversion a Exceso-3 y Relleno (con digito extra)" << endl;
    cout << "A: " << setw(8) << num1_int << " en Exceso-3 -> " << ex3_a << endl;
    cout << "B: " << setw(8) << num2_int << " en Exceso-3 -> " << ex3_b << endl;
    cout << "------------------------------------------" << endl;

    // A - B = A + C9(B) + 1
    
    cout << "Paso 2: Calcular C9 de B (Invertir bits)" << endl;
    string c9_b = invertir_bits(ex3_b);
    cout << "C9(" << ex3_b << ") -> " << c9_b << endl;

    cout << "\nPaso 3: Sumar A + C9(B) + 1 (El +1 entra como acarreo inicial)" << endl;
    
    int carry = 1; // El "+1" del C10
    string resultado_final_ex3 = "";
    int num_grupos = ex3_a.length() / 4;

    for (int i = num_grupos - 1; i >= 0; --i) {
        string digito_a_str = ex3_a.substr(i * 4, 4);
        string digito_b_str = c9_b.substr(i * 4, 4); 

        int digito_a = bin_a_int(digito_a_str);
        int digito_b = bin_a_int(digito_b_str);

        cout << "\nAnalizando Digito " << (num_grupos - i) << " (derecha a izquierda):" << endl;
        cout << "  " << digito_a_str << " (A)" << endl;
        cout << "+ " << digito_b_str << " (C9 de B)" << endl;
        cout << "+ " << setw(4) << setfill('0') << carry << " (Acarreo anterior)" << endl;
        cout << "  ----" << endl;

        // 1. Suma binaria
        int suma_parcial = digito_a + digito_b + carry;

        if (suma_parcial > 15) {
            carry = 1;
            suma_parcial -= 16;
        } else {
            carry = 0;
        }

        cout << "  = " << int_a_bin4(suma_parcial) << " (" << suma_parcial << ") [Acarreo binario: " << carry << "]" << endl;

        // 2. Corrección Exceso-3 (Idéntica a la de la suma)
        int suma_corregida;
        if (carry == 1) {
            cout << "  -> HUBO ACARREO. Correccion: Sumar 3 (0011)" << endl;
            suma_corregida = suma_parcial + 3;
        } else {
            cout << "  -> NO HUBO ACARREO. Correccion: Restar 3 (0011)" << endl;
            suma_corregida = suma_parcial - 3;
        }
        
        resultado_final_ex3.insert(0, int_a_bin4(suma_corregida));
        cout << "  = " << int_a_bin4(suma_corregida) << " (Resultado Corregido del Digito)" << endl;
        cout << "------------------------------------------" << endl;
    }
    
    // Paso 4: Interpretar el resultado
    cout << "\nPaso 4: Interpretar Acarreo Final" << endl;
    
    int resultado_int;
    string magnitud_ex3;

    if (carry == 1) {
        cout << "  -> HUBO ACARREO FINAL (Carry = 1)." << endl;
        cout << "  -> El resultado es POSITIVO." << endl;
        cout << "  -> Se descarta el acarreo." << endl;
        
        magnitud_ex3 = resultado_final_ex3;
        resultado_int = exceso3_a_decimal(magnitud_ex3);

    } else {
        cout << "  -> NO HUBO ACARREO FINAL (Carry = 0)." << endl;
        cout << "  -> El resultado es NEGATIVO y esta en C9(Exceso-3)." << endl;
        cout << "  -> Resultado parcial (en C9): " << resultado_final_ex3 << endl;

        cout << "\nPaso 5: Invertir bits para obtener magnitud en Exceso-3" << endl;
        magnitud_ex3 = invertir_bits(resultado_final_ex3);
        
        resultado_int = -exceso3_a_decimal(magnitud_ex3);
    }

    cout << "\n=== Resultado Resta Exceso-3 ===" << endl;
    cout << "Resultado en Exceso-3 (Magnitud): " << magnitud_ex3 << endl;
    cout << "Resultado en Decimal: " << resultado_int << endl;

    if (resultado_int == num1_int - num2_int) {
        cout << "(Verificacion: CORRECTO)" << endl;
    } else {
        cout << "(Verificacion: INCORRECTO. Esperado: " << num1_int - num2_int << ")" << endl;
    }
}


// --- Función Principal ---

int main() {
    // Fase 1
    simular_suma_bcd();

    // Fase 2
    simular_resta_bcd();
    
    // Fase 3
    simular_suma_exceso3();

    // Fase 4
    simular_resta_exceso3();

    cout << "\n... Todas las simulaciones completadas. Presiona Enter para salir." << endl;
    cin.ignore(); // Limpia el buffer de entrada
    cin.get();    // Espera a que el usuario presione Enter
    
    return 0;
}