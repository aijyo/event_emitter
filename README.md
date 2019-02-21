# event_emitter
fix me up
1) Didn't support callback with return value, because I'm cannot find a suitable way to 
distinguish between "void emit(const EventType& eventid, Ret& value, Args&& ... args)" 
and "void emit(const EventType& eventid, Args&& ... args)" .

2) Mutithreading is not support now...

3) If a callback has set to be callonce, so remove it,(not impl...)

4) ...