

import java.io.*;
import java.util.HashSet;
import java.util.Map;
import java.util.Scanner;
import java.util.TreeMap;

public class NaiveBayes {

	//every NaiveBayes classifier should have fields to document test data
	Integer totalEmails = 0;
	Integer spamEmails = 0;
	Integer hamEmails = 0;
	//stores every word seen in test emails, case sensitive 
	Map<String, Integer> hamMap = new TreeMap<String, Integer>();
	Map<String, Integer> spamMap = new TreeMap<String, Integer>();

	/*
	 * !! DO NOT CHANGE METHOD HEADER !! 
	 */
	public void train(File[] hams, File[] spams) throws IOException {

		// check every file passed
		// start with the ham, ham > spam, in my opinion
		for (File file : hams) {
			// get all the words
			HashSet<String> fileWords = tokenSet(file);
			// either add the word to hamMap, or increase the number of ham emails containing word
			for (String word : fileWords) {
				if (!this.hamMap.containsKey(word)) {
					this.hamMap.put(word, 1);
				} else {
					Integer i = this.hamMap.get(word);
					i++;
					this.hamMap.put(word, i);
				}
			}
			this.hamEmails++;
			this.totalEmails++;
		}
		// now for the spam
		for (File file : spams) {
			//get all the words
			HashSet<String> fileWords = tokenSet(file);
			// either add the word to spamMap, or increase the number of spam emails containing word
			for (String word : fileWords) {
				if (!this.spamMap.containsKey(word)) {
					this.spamMap.put(word, 1);
				} else {
					Integer i = this.spamMap.get(word);
					i++;
					this.spamMap.put(word, i);
				}
			}
			this.spamEmails++;
			this.totalEmails++;
		}
	}

	/*
	 * !! DO NOT CHANGE METHOD HEADER !! 
	 *
	 * Classify the given unlabeled set of emails. Follow the format in
	 * example_output.txt and output your result to stdout. Note the order of the
	 * emails in the output does NOT matter.
	 * 
	 *
	 * Params: emails - unlabeled email files to be classified
	 */
	public void classify(File[] emails) throws IOException {

		//get P(Spam) and P(Ham)
		double unconProbEmailSpam = ((double)this.spamEmails) / ((double)this.totalEmails);
		double unconProbEmailHam = ((double)this.hamEmails) / ((double)this.totalEmails);
		//using Naive Bayes theorem requires equation smoothing
		double spamSmoothingDenom = this.spamEmails + 2;
		double hamSmoothingDenom = this.hamEmails + 2;

		// go through every e-mail
		for (File email : emails) {

			// initial the numerators of P(Spam|words) and P(Ham|Words) 
			//Log Summation -> start at zero
			double pEmailSpam = 0;
			double pEmailHam = 0;
			String fileName;
			// get the e-mail's set of words
			HashSet<String> emailsWords = tokenSet(email);

			// for every word, evaluate P(Word|Spam) and P(Word|Ham) 
			for (String word : emailsWords) {

				// this words Probability
				double pXiGivenSpam;
				double pXiGivenHam;

				// set up the numerators, Naive Bayes Theorem requires adding 1 for smoothing
				double spamNum = 1;
				double hamNum = 1;

				// add to numerators the number of words occurrences in trial e-mail
				if (spamMap.containsKey(word)) {
					spamNum += spamMap.get(word);
				}
				//if, not else if
				if (hamMap.containsKey(word)) {
					hamNum += hamMap.get(word);
				}
				
				//P(word|spam) = number of spam e-mails containing word, divided by the total number of spam e-mails, comparatively for P(word|ham)
				pXiGivenSpam = spamNum / spamSmoothingDenom;
				pXiGivenHam = hamNum / hamSmoothingDenom;
				
				//avoiding underflow by using summation of log(value), rather then product of the value
				double x = Math.log(pXiGivenSpam);
				double y = Math.log(pXiGivenHam);
				pEmailSpam += x;
				pEmailHam += y;
			}
			
			// we have evaluated every word in the email
			// find its probability of being spam and ham, = log(P(spam)) + (summation of log(word|spam)), comparatively for ham
			Double pSpamGivenWords = Math.log(unconProbEmailSpam) + pEmailSpam;
			Double pHamGivenWords = Math.log(unconProbEmailHam) + pEmailHam;

			fileName = email.getName();
			if (pSpamGivenWords > pHamGivenWords) {
				System.out.println(fileName + " spam");
			} else {
				 System.out.println(fileName + " ham");
			}
		}
	}

	/*
	 * Helper Function: This function reads in a file and returns a set of all the
	 * tokens. It ignores "Subject:" in the subject line.
	 * 
	 * If the email had the following content:
	 * 
	 * Subject: Get rid of your student loans Hi there , If you work for us , we
	 * will give you money to repay your student loans . You will be debt free !
	 * FakePerson_22393
	 * 
	 * This function would return to you ['be', 'student', 'for', 'your', 'rid',
	 * 'we', 'of', 'free', 'you', 'us', 'Hi', 'give', '!', 'repay', 'will', 'loans',
	 * 'work', 'FakePerson_22393', ',', '.', 'money', 'Get', 'there', 'to', 'If',
	 * 'debt', 'You']
	 */
	public static HashSet<String> tokenSet(File filename) throws IOException {
		HashSet<String> tokens = new HashSet<String>();
		Scanner filescan = new Scanner(filename);
		filescan.next(); // Ignoring "Subject"
		while (filescan.hasNextLine() && filescan.hasNext()) {
			tokens.add(filescan.next());
		}
		filescan.close();
		return tokens;
	}
}
