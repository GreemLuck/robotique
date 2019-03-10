import os
import sys
import zipfile
import shutil

if len(sys.argv) == 2:
	filename = sys.argv[1]
	base_name, extension = os.path.splitext(filename)
	base_name_split = base_name.split("_")

	# The filename must consist of three parts separated by underscores
	if len(base_name_split) != 3:
		print("Your file does not follow the naming convention gg_nn_ff.zip")
		exit(0)

	group_number = base_name_split[0]
	report_name = base_name + ".pdf"
	temp_directory = base_name + "_temporary_archive_check/"

	### reset temporary directory
	if os.path.exists(temp_directory):
		shutil.rmtree(temp_directory)

	### access specified archive
	try:
		zip_file = zipfile.ZipFile(filename, 'r')
		zip_file.extractall(temp_directory)
		zip_file.close()
	except:
		print("Your archive can not be decompressed. Make sure there is nothing wrong with the file.")
		exit(0)

	### show name
	print("\n\n=== (1/3) Verify the correct use of the naming convention (gg_nn_ff.zip) below: ===")
	print("\tYour group number [gg]:\t" + group_number)
	print("\tYour first name [ff]:\t" + base_name_split[2])
	print("\tYour last name [nn]:\t" + base_name_split[1])

	### show archive content
	print("\n=== (2/3) Verify the content of your archive (controllers, worlds, gg_nn_ff.pdf) below: ===")
	print("\tFiles in your archive's root:")
	archive_content = os.listdir(temp_directory)
	for file in archive_content:
		if not file in ('controllers', 'worlds', report_name, 'misc'):
			print("\t\t" + file + " --> files other than the controllers, the worlds and the report ought to be placed in a directory misc/")
		else:
			print("\t\t" + file)

	print("")

	### automatically hand-in guidelines
	print("=== (3/3) If there are any errors shown below this line, your submission will not be considered for evaluation. Otherwise, you can submit pour archive. ===")
	# The provided file must be a zip archive
	if extension != '.zip':
		print("\tError: Your archive must be a .zip file")

	# The first part of the filename must be an integer value between 00 and 99 to specify the group number
	try:
		int(group_number)
	except:
		print("\tError: Your group number ('" + group_number + "') is not a valid integer")
	if len(group_number) != 2:
		print("\tError: Your group number ('" + group_number + "') is not a valid integer of length two")

	# The archive's root must contain a non-empty controllers directory
	if "controllers" not in archive_content:
		print("\tError: You have not provided a controllers directory at the root of your archive")
		exit(0)
	if len(os.listdir(temp_directory + "controllers")) == 0:
		print("\tWarning: Your controllers directory is empty")

	# The archive's root must contain a non-empty worlds directory
	if "worlds" not in archive_content:
		print("\tError: You have not provided a worlds directory at the root of your archive")
		exit(0)
	if len(os.listdir(temp_directory + "worlds")) == 0:
		print("\tWarning: Your worlds directory is empty")

	# The archive's root must contain your report with the naming convention gg_nn_ff.pdf
	if report_name not in archive_content:
		print("\tError: Your report is either missing or does not use the same naming convention as your archive")

	print("")
	### cleanup
	shutil.rmtree(temp_directory)

else:
	print("Place this script in the same directory as your zip file and provide its name as an argument (e.g. call: python student_script.py 99_mustermann_max.zip)\n")