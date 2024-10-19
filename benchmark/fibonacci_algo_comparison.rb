require "benchmark/ips"

def recursive(n)
  return 1 if n == 1
  return 0 if n <= 0

  recursive(n - 1) + recursive(n - 2)
end

def iterative(n)
  first = 0
  second = 1
  (1..n-1).each do |i|
    second = first + second
    first = second - first
  end

  second
end

n = ARGV[0].to_i
# recursive(n)
# iterative(n)

# Benchmark.ips do |x|
#   x.report("recursive") { recursive(n) }
#   x.report("iterative") { iterative(n) }
#   x.compare!
# end

puts iterative(n)

