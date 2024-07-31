
bazel build //examples/helloworld/...
bazel build //examples/features/fiber_forward/...

echo "begin"
./bazel-bin/examples/helloworld/helloworld_svr --config=./examples/helloworld/conf/trpc_cpp_fiber.yaml &
sleep 1
./bazel-bin/examples/features/fiber_forward/proxy/fiber_forward --config=./examples/features/fiber_forward/proxy/trpc_cpp_fiber.yaml &
sleep 1
./bazel-bin/examples/features/fiber_forward/client/client --client_config=./examples/features/fiber_forward/client/trpc_cpp_fiber.yaml
killall helloworld_svr