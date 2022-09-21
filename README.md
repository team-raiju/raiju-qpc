## QP-Tools-Tests

### Configuration

Dowload QPC bundle from https://www.state-machine.com/ (This will include QP/C, QP/C++ and QTools). Or you can download separatadely from github https://github.com/QuantumLeaps


After that include QPC path to environment: ```export QPC="/home/user/qp-linux_7.1.0/qp/qpc```.
Also add QTOOLS as follows: ```export QTOOLS="/home/user/qp-linux_7.1.0/qp/qtools``` and ```export PATH="/home/user/qp-linux_7.1.0/qp/qtools/bin:$PATH"```

Lastly, it necessary to build qspy tools into qp-linux_7.1.0/qp/qtools/bin folder.
* ```cd ~/qp-linux_7.1.0/qp/qtools/qspy/posix/```
* ```make```

This will automatically build qspy into qp/qtools/bin folder.

The last tool needed is qview. You can install via ```pip install qview```, or you can use as a standalone file that is located ```cd ~/qp-linux_7.1.0/qp/qtools/qview/```.

Qview requires python tkinter: ```sudo apt-get install python3-tk```