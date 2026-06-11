# GasSensor
System for gas monitoring and high-voltage interlock

Per monitorare la condizione del gas all'interno della camera utilizziamo Grafana per costruire il log.
Per avere il monitor attivo:
- avviare Influxd da Powershell
```cd -Path 'C:\Program Files (x86)\influxdb'```
```.\influxd.exe```
- attivare il log delle correnti comunicando con il mainframe
sempre da Powershell:
```cd -Path 'F:\wd\hv_monitor\'```
```python3 .\hv_logger_SY4527.py```
- aprire Grafana sul Browser all'indirizzo:  http://localhost:3000/  (username e password sono *admin* e *admin*)