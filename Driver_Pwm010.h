/*
 * Driver_Pwm010.h
 *
 *  Created on: Abr 2018
 *      Author: raulMrello
 *
 *	Driver_Pwm010 es el módulo encargado de gestionar la activación y apagado de un Driver para luminarias
 *	con control 0V-10V mediante PWM. Es posible generar patrones con encendidos y apagados instantáneos o
 *	en rampa.
 *
 */
 
#ifndef __Driver_Pwm010__H
#define __Driver_Pwm010__H

#include "mbed.h"

   
class Driver_Pwm010{
  public:

    /** Configuración para establecer la lógica de activación */
	enum Driver_Pwm010LogicLevel{
		OnIsLowLevel,
		OnIsHighLevel
	};
		
	/** Constructor
     *  @param Driver_Pwm010 GPIO conectado al Driver_Pwm010
     *  @param level Nivel de activación lógico 
     *  @param period Periodo del del pwm en milisegundos
     */
    Driver_Pwm010(PinName Driver_Pwm010, Driver_Pwm010LogicLevel level = OnIsHighLevel, uint32_t period_ms = 1, bool debug=false);
    ~Driver_Pwm010();


	/** Establece el factor de escaña aplicado a nivel de activación. Por ejemplo con un factor de
	 *  escala de 0.8 implica que la ejecución 'setLevel(100)' se traduzca en 'setLevel(0.8 * 100)'
     *  @param scale Factor de escala
	 */
    void setScaleFactor(double scale) { _scale = scale; }
  
  
	/** Inicia un cambio de nivel, con o sin rampa incial
     *  @param intensity Intensidad en porcentaje 0-100%
	 *	@param ms_ramp Tiempo hasta alcanzar la intensidad (0: instantánea, !=0: millisegundos)
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
    double _next_level;                                     /// Próximo nivel de intesidad
    double _step_level;										/// Paso de incremento/decremento de intensidad
    Driver_Pwm010LogicLevel _level;                         /// Nivel lógico para la activación
    Driver_Pwm010Stat _stat;                                /// Estado del Driver_Pwm010
    Driver_Pwm010Action _action;                            /// Acción en ejecución del Driver_Pwm010
    uint32_t _period_ms;                                    /// Periodo del pwm en milisegundos
    Ticker _tick_ramp;                                      /// Timer para la rampa
    uint32_t _ms_ramp;                                      /// Milisegundos de rampa
    Driver_Pwm010Stat _bkp_stat;                            /// Estado backup en modo temporal
    bool  _debug;                                           /// Canal de depuración
  
    
	/** Callback para ir al nivel seleccionado de forma gradual
     */
    void rampCb();
  
    
	/** convertIntensity
     *  Convierte la intensidad 0-100% en un valor de 0-1 teniendo en cuenta la lógica
     *  @param intensity Intensidad 0-100%
     *  @return Intensidad 0 - 1.0f
     */
    double convertIntensity(uint8_t intensity);
};
     


#endif /*__Driver_Pwm010__H */

/**** END OF FILE ****/


