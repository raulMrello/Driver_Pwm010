/*
 * Driver_Pwm010.cpp
 *
 *  Created on: Abr 2018
 *      Author: raulMrello
 */

#include "Driver_Pwm010.h"



//------------------------------------------------------------------------------------
//--- PRIVATE TYPES ------------------------------------------------------------------
//------------------------------------------------------------------------------------


#define NULL_CALLBACK               (void(*)())0

/** Macro para imprimir trazas de depuración, siempre que se haya configurado un objeto
 *	Logger válido (ej: _debug)
 */
static const char* _MODULE_ = "[RlyMan]........";
#define _EXPR_	(_debug && !IS_ISR())

 
//------------------------------------------------------------------------------------
//-- PUBLIC METHODS IMPLEMENTATION ---------------------------------------------------
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
Driver_Pwm010::Driver_Pwm010(PinName gpio, Driver_Pwm010LogicLevel level, uint32_t period_ms, bool debug){
    // Crea objeto
    _id = (uint32_t)gpio;
    _debug = debug;
    _period_ms = period_ms;
    _scale = 1.0;
    
    _out = new PwmOut(gpio, MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1A);
    _out->period_ms(_period_ms);
    _level = level;      
    
    // Deja apagado por defecto
    setLevel(0);
}


//------------------------------------------------------------------------------------
void Driver_Pwm010::setLevel(uint8_t intensity, uint32_t ms_ramp){
	_tick_ramp.detach();
    // Si no hay rampa...
    if(ms_ramp == 0){
        _stat = Driver_Pwm010HasReachedLevel;
        _next_level = convertIntensity(intensity);
        DEBUG_TRACE_D(_EXPR_, _MODULE_, "Conversión de %d a %.2f", intensity, _next_level);
        _out->write(_next_level);
		_curr_level = _next_level;
    }
    // si hay rampa, la inicia
    else{
        _stat = Driver_Pwm010IsGoingLevel;
        _next_level = convertIntensity(intensity);
        DEBUG_TRACE_D(_EXPR_, _MODULE_, "Conversión de %d a %.2f", intensity, _next_level);
        _step_level = (_next_level - _curr_level)/DefaultNumStepsPerRamp;
        _tick_ramp.attach_us(callback(this, &Driver_Pwm010::rampCb), (ms_ramp * 1000)/DefaultNumStepsPerRamp);
    }    
}


 
//------------------------------------------------------------------------------------
//-- PRIVATE METHODS IMPLEMENTATION --------------------------------------------------
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
void Driver_Pwm010::rampCb(){
	_curr_level += _step_level;
	_out->write(_curr_level);
	if((_step_level >= 0 && _curr_level >= _next_level) || (_step_level < 0 && _curr_level <= _next_level)){
		_tick_ramp.detach();
		_stat = Driver_Pwm010HasReachedLevel;
	}
}


//------------------------------------------------------------------------------------
double Driver_Pwm010::convertIntensity(uint8_t intensity){
    intensity = (intensity > 100)? 100 : intensity;
    double fint;
    if(_level == OnIsHighLevel){
    	fint = (intensity > 0)? (((double)intensity)/100) : 0;
    	fint = _scale * fint;
    }
    else{
    	fint = (intensity < 100)? (((double)intensity)/100) : 1.0f;
    	fint = _scale * (1.0f - fint);
    }
    return((fint <= 0)? 0 : fint);
}
