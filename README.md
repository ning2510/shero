### shero

coroutine 版本

```
$ ./bin/test_http_server

# QPS 还降低了...
# -DBENCH_MARK
$ wrk -c 1000 -t 8 -d 30 --latency 'http://10.13.0.39:9999'
Running 30s test @ http://10.13.0.39:9999
  8 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    15.01ms   11.02ms 432.93ms   88.03%
    Req/Sec     8.54k     1.12k   20.84k    87.88%
  Latency Distribution
     50%   14.96ms
     75%   15.53ms
     90%   16.41ms
     99%   39.55ms
  2040008 requests in 30.07s, 383.26MB read
Requests/sec:  67836.59
Transfer/sec:     12.74MB
```