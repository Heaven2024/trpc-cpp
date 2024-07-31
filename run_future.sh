bazel build //examples/helloworld/...
bazel build //examples/features/future_forward/...

./bazel-bin/examples/helloworld/helloworld_svr --config=./examples/helloworld/conf/trpc_cpp.yaml &
sleep 1
./bazel-bin/examples/features/future_forward/proxy/future_forward --config=./examples/features/future_forward/proxy/trpc_cpp_future.yaml &
sleep 1
./bazel-bin/examples/features/future_forward/client/client --client_config=./examples/features/future_forward/client/trpc_cpp_future.yaml
killall helloworld_svr
