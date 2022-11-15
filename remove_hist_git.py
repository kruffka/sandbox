import subprocess

# p = subprocess.Popen('git log', shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
# # for line in p.stdout.readlines():
# #     print(line)

# output = p.stdout.readlines()
# searchin_for = b'commit 306d2619277b0e96aac3903d24b5c3900c55165b\n'

# row = 0
# for line in output:
#     # print(line)
#     if (line == searchin_for):
#         # print("remove from =", row)
#         break

#     row += 1


# remove_from = output[row:]
# # print(output2)
# # for line in output2:
# #     print(line)

# subprocess.Popen("rm .git/info/grafts", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

# for line in remove_from:
#     if (line[:6] == b'commit'):
#         commit = line[7:-1].decode("utf-8")
#         command = 'echo "' + commit + '" >> .git/info/grafts'
#         print(command)
#         subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)



commit = '9aada422403cb2ffac32b26ec7e5ea68ce297799'
command = 'echo "' + commit + '" > .git/info/grafts'
subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

# git filter-branch -- --all
print("done!")