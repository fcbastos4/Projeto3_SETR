/** \file sensor_temp.h
 *  \brief Este módulo é responsável por declarar as funções relacionadas ao sensor de temperatura TC74.
 * 
 *  Este ficheiro contém a declaração da função get_temp(), que é responsável por ler a temperatura do sensor TC74 via I2C.
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */

#ifndef SENSOR_TEMP_H
#define SENSOR_TEMP_H

/** \brief Função que lê a temperatura do sensor TC74.
 * 
 *  A função get_temp() retorna a temperatura lida do sensor, ou um erro fatal se a comunicação falhar.
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */
int get_temp(void);  

#endif // SENSOR_TEMP_H