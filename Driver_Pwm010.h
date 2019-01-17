/*
 * Driver_Pwm010.h
 *
 *  Created on: Abr 2018
 *      Author: raulMrello
 *
 *	Driver_Pwm010 es el m�dulo encargado de gestionar la activaci�n y apagado de un Driver para luminarias
 *	con control 0V-10V mediante PWM. Es posible generar patrones con encendidos y apagados instant�neos o
 *	en rampa.
 *
 */
 
#ifndef __Driver_Pwm010__H
#define __Driver_Pwm010__H

#include "mbed.h"

   
class Driver_Pwm010{
  public:

    /** Configuraci�n para establecer la l�gica de activaci�n */
	enum Driver_Pwm010LogicLevel{
		OnIsLowLevel,
		OnIsHighLevel
	};
		
	/** Constructor
     *  @param Driver_Pwm010 GPIO conectado al Driver_Pwm010
     *  @param level Nivel de activaci�n l�gico 
     *  @param period Periodo del del pwm en milisegundos
     */
    Driver_Pwm010(PinName Driver_Pwm010, Driver_Pwm010LogicLevel level = OnIsHighLevel, uint32_t period_ms = 1, bool debug=false);
    ~Driver_Pwm010();


	/** Establece el factor de esca�a aplicado a nivel de activaci�n. Por ejemplo con un factor de
	 *  escala de 0.8 implica que la ejecuci�n 'setLevel(100)' se traduzca en 'setLevel(0.8 * 100)'
     *  @param scale Factor de escala
	 */
    void setScaleFactor(double scale) { _scale = scale; }
  
  
	/** Inicia un cambio de nivel, con o sin rampa incial
     *  @param intensity Intensidad en porcentaje 0-100%
	 *	@param ms_ramp Tiempo hasta alcanzar la intensidad (0: instant�nea, !=0: millisegundos)
	 */
    void setLevel(uint8_t level, uint32_t ms_ramp = 0);

         
  private:
    enum Driver_Pwm010Stat{
        Driver_Pwm010IsGoingLevel,
        Driver_Pwm010HasReachedLevel,
    };
    enum Driver_Pwm010Action{
        Driver_Pwm010GoingLevel,
		Driver_Pwm010ReachedLevel,
    };
    
    static const uint8_t DefaultNumStepsPerRamp = 100;		/// Las rampas se hacen en 100 pasos
    static const uint32_t GlitchFilterTimeoutUs = 20000;    /// Por defecto 20ms de timeout antiglitch desde el cambio de nivel
    uint32_t _id;                                           /// Driver_Pwm010 id. Coincide con el PinName asociado
    PwmOut* _out;                                           /// Salida pwm
    double _scale;											/// Factor de escala
    double _curr_level;                                     /// Nivel de intensidad actual
    double _next_level;                                     /// Pr�ximo nivel de intesidad
    double _step_level;										/// Paso de incremento/decremento de intensidad
    Driver_Pwm010LogicLevel _level;                         /// Nivel l�gico para la activaci�n
    Driver_Pwm010Stat _stat;                                /// Estado del Driver_Pwm010
    Driver_Pwm010Action _action;                            /// Acci�n en ejecuci�n del Driver_Pwm010
    uint32_t _period_ms;                                    /// Periodo del pwm en milisegundos
    Ticker _tick_ramp;                                      /// Timer para la rampa
    uint32_t _ms_ramp;                                      /// Milisegundos de rampa
    Driver_Pwm010Stat _bkp_stat;                            /// Estado backup en modo temporal
    bool  _debug;                                           /// Canal de depuraci�n
  
    
	/** Callback para ir al nivel seleccionado de forma gradual
     */
    void rampCb();
  
    
	/** convertIntensity
     *  Convierte la intensidad 0-100% en un valor de 0-1 teniendo en cuenta la l�gica
     *  @param intensity Intensidad 0-100%
     *  @return Intensidad 0 - 1.0f
     */
    double convertIntensity(uint8_t intensity);
};
     


#endif /*__Driver_Pwm010__H */

/**** END OF FILE ****/


