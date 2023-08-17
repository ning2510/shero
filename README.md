shero

bench mark
```
$ ./bin/test_http_server

# -DBENCH_MARK
$ wrk -c 1000 -t 8 -d 30 --latency 'http://127.0.0.1:9999'
Running 30s test @ http://127.0.0.1:9999
  8 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     6.75ms    1.99ms 221.26ms   96.56%
    Req/Sec    18.35k     1.73k   25.09k    89.92%
  Latency Distribution
     50%    6.60ms
     75%    6.71ms
     90%    6.97ms
     99%   10.91ms
  4383753 requests in 30.07s, 823.59MB read
Requests/sec: 145781.07
Transfer/sec:     27.39MB

# 测压前一定要更改日志等级!
$ wrk -c 1000 -t 8 -d 30 --latency 'http://127.0.0.1:9999'
Running 30s test @ http://127.0.0.1:9999
  8 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    17.80ms    8.03ms 256.20ms   87.18%
    Req/Sec     6.96k     1.46k   16.39k    77.97%
  Latency Distribution
     50%   16.24ms
     75%   17.98ms
     90%   24.39ms
     99%   44.80ms
  1643613 requests in 30.09s, 308.79MB read
Requests/sec:  54618.72
Transfer/sec:     10.26MB
```