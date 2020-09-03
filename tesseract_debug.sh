#!/bin/bash
tesseract --print-parameters
tesseract --tessdata-dir tessdata client/storage/detected_codes/detected_code_$1.jpg tessout tessconfig
