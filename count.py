import sys
weight = [ 0.274, 1.35, 5.902 ]

if __name__ == "__main__":
  argvs = sys.argv
  argc = len(argvs)
  if argc != 5:
    print "usage: python count [S] [M] [L] [total weight]"
    sys.exit(-1)

  raito = [int(argvs[1]), int(argvs[2]), int(argvs[3])]
  total_weight = int(argvs[4])
  raito_total = raito[0] * weight[0] + raito[1] * weight[1] + raito[2] * weight[2];
  s = int(raito[0]*(total_weight/raito_total)+0.5)
  m = int(raito[1]*(total_weight/raito_total)+0.5)
  l = int(raito[2]*(total_weight/raito_total)+0.5)
  t = s * weight[0] + m * weight[1] + l * weight[2];
  print "---result---"
  print "small:  " + str(s)
  print "medium: " + str(m)
  print "large:  " + str(l)
  print "  total:" + str(t) + "g"
  sys.exit(0)
