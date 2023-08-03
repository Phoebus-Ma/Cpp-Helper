
## Introduction

CpuUsageMonitor is a monitor cpu usage application via Windows WMI Service.
Compared to the task manager, the numbers produced by this software are not very accurate.


## Theory

1. Run the following command in PowerShell for get PercentProcessorTime value:

```bash
$ Get-CimInstance -ClassName Win32_PerfFormattedData_PerfOS_Processor | Select-Object PercentProcessorTime

PercentProcessorTime
--------------------
                  35
                   6
                   6
                   0
                   0
                   0
                  12
                   0
                   7
```

This is a 8 cores CPU data:

- Core 0: 35    (%)
- Core 1: 06    (%)
- Core 2: 06    (%)
- Core 3: 00    (%)
- Core 4: 00    (%)
- Core 5: 00    (%)
- Core 6: 12    (%)
- Core 7: 00    (%)
- _Total: 07    (%)

```bash
((35 + 6 + 6 + 0 + 0 + 0 + 12 + 0) / 8) == (float)7.375 == (int)7;
```

2. Run the following command in PowerShell for get PercentIdleTime value:

```bash
$ Get-CimInstance -ClassName Win32_PerfFormattedData_PerfOS_Processor | Select-Object PercentIdleTime
```

(100 - PercentIdleTime value) probably closer to the Task Manager cpu usage value.


## Platform

Windows 10+.

Visual Studio 2022.
