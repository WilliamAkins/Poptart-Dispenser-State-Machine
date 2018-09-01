//Completed by William Akins | 15029476
#include <iostream>
#include <vector>
#include <string>

using namespace std;

//enum is used to set both the current state and the different parameters for the dispenser
enum state { Out_Of_Poptart, No_Credit, Has_Credit, Dispenses_Poptart };
enum stateParameter { No_Of_Poptarts, Credit, Cost_Of_Poptart };

//stateContext has been forward referenced as it will be used within the state class while being declared after it
class StateContext;

class State {
	//This class holds the current state that the dispenser is in. It's a generic class that is not specific to a popart dispenser
	protected:
		StateContext* CurrentContext;

	public:
		//gets the reference to the statContext with a pointer
		State(StateContext* Context) {
			CurrentContext = Context;
		}
		virtual ~State(void) {}
		virtual void transition(void) {}
};

class StateContext {
	//This is what physically holds what the current state is and will always be used when transitioning between states
	//It has both the getters and setters for the state and state parameters, which 
	//It is also a generic class that is not specific to the poptart dispenser
	protected:
		State* CurrentState = nullptr;
		int stateIndex = 0;
		vector<State*> availableStates;
		vector<int> stateParameters;

	public:
		virtual ~StateContext(void);
		virtual void setState(state newState);
		virtual int getStateIndex(void);
		virtual void setStateParam(stateParameter SP, int value);
		virtual int getStateParam(stateParameter SP);
};

StateContext::~StateContext(void) {
	//destroys the state and the state parameters when the stateContext destructs to prevent any memory leaks
	for (int i = 0; i < this->availableStates.size(); i++) delete this->availableStates[i];
	this->availableStates.clear();
	this->stateParameters.clear();
}

void StateContext::setState(state newState) {
	//sets the current state to the new state that was passed into the method
	this->CurrentState = availableStates[newState];
	this->stateIndex = newState;
	this->CurrentState->transition();
}

int StateContext::getStateIndex(void) {
	return this->stateIndex;
}

void StateContext::setStateParam(stateParameter SP, int value) {
	//sets the current state parameter based off of the arguements passed into it, however this version is not used but rather the poptart specific one
	//that is inherited from it
	this->stateParameters[SP] = value;
}

int StateContext::getStateParam(stateParameter SP) {
	return this->stateParameters[SP];
}

class Transition {
	//each state has a set of associated methods, they will output an error message if not properly initialised rather than crashing
	public:
		virtual bool insertMoney(int) { cout << "Error!" << endl; return false; }
		virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }
		virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; }
		virtual bool addPoptart(int) { cout << "Error!" << endl; return false; }
		virtual bool dispense(void) { cout << "Error!" << endl; return false; }
};

class PoptartState : public State, public Transition {
	//This class takes inherits from the generic state and transition class and begins to apply poptart specific functionality to the states
	public:
		PoptartState(StateContext* Context) : State(Context) {}
};

#pragma region Poptart state classes
//These are the state class definition for each poptart dispenser specific classes
class OutOfPoptart : public PoptartState {
	public:
		OutOfPoptart(StateContext* Context) : PoptartState(Context) {}
		bool insertMoney(int money);
		bool makeSelection(int option);
		bool moneyRejected(void);
		bool addPoptart(int number);
		bool dispense(void);
};

class NoCredit : public PoptartState {
	public:
		NoCredit(StateContext* Context) : PoptartState(Context) {}
		bool insertMoney(int money);
		bool makeSelection(int option);
		bool moneyRejected(void);
		bool addPoptart(int number);
		bool dispense(void);
};

class HasCredit : public PoptartState {
	public:
		HasCredit(StateContext* Context) : PoptartState(Context) {}
		bool insertMoney(int money);
		bool makeSelection(int option);
		bool moneyRejected(void);
		bool addPoptart(int number);
		bool dispense(void);
};

class DispensesPoptart : public PoptartState {
	public:
		DispensesPoptart(StateContext* Context) : PoptartState(Context) {}
		bool insertMoney(int money);
		bool makeSelection(int option);
		bool moneyRejected(void);
		bool addPoptart(int number);
		bool dispense(void);
};
#pragma endregion

#pragma region Product
class Product {
	friend class Filling;
	//The homogeneous product state that works with the factory method design pattern
	protected:
		string product_description;
		int itemCost = 0;
	public:
		virtual void consume(void);
		virtual int cost(void);
		virtual string description(void);
};

void Product::consume(void) {
	//delete the product once it has been dispensed, this will prevent any memory leaks from occuring
	delete this;
}

int Product::cost(void) {
	return this->itemCost;
}

string Product::description(void) {
	return this->product_description;
}
//Makes use of the factory method pettern where each individual poptart will have it's own class that inherits from the
//homogeneous poptart and product class.
class Poptart : public Product {
	public:
		Poptart(void) {
			this->product_description = "Poptart";
			this->itemCost = 50;
		}
};

class Plain : public Poptart {
	public:
		Plain(void) {
			this->product_description = "Plain";
			this->itemCost = 100;
		}
};

class Spicy : public Poptart {
	public:
		Spicy(void) {
			this->product_description = "Spicy";
			this->itemCost = 150;
		}
};

class Chocolate : public Poptart {
	public:
		Chocolate(void) {
			this->product_description = "Chocolate";
			this->itemCost = 200;
		}
};

class Coconut : public Poptart {
public:
	Coconut(void) {
		this->product_description = "Coconut";
		this->itemCost = 200;
	}
};

class Fruity : public Poptart {
	public:
		Fruity(void) {
			this->product_description = "Fruity";
			this->itemCost = 200;
		}
};
#pragma endregion

#pragma region Poptart_Dispenser
class Poptart_Dispenser : public StateContext, public Transition {
	friend class DispensesPoptart;
	friend class HasCredit;

private:
	PoptartState* PoptartCurrentState = nullptr;
	bool itemDispensed = false;
	//indicates whether a product is there to be retrieved
	Product* DispensedItem = nullptr;
	bool itemRetrieved = false; //indicates whether a product has been retrieved
public:
	Poptart_Dispenser(int inventory_count);
	~Poptart_Dispenser(void);
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
	Product* getProduct(void);
	virtual void setStateParam(stateParameter SP, int value);
	virtual int getStateParam(stateParameter SP);
};

Poptart_Dispenser::Poptart_Dispenser(int inventory_count) {
	//Creates an instance of each state
	this->availableStates.push_back(new OutOfPoptart(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesPoptart(this));

	this->stateParameters.push_back(0); //Number of poptarts
	this->stateParameters.push_back(0); //Credit
	this->stateParameters.push_back(0); //Cost of poptarts

	//sets the initial state of the poptart dispenser to "Out of Poptarts"
	this->setState(Out_Of_Poptart);

	//if there are any poptarts, then add them to the dispenser
	if (inventory_count > 0) {
		this->addPoptart(inventory_count);
	}
}

Poptart_Dispenser::~Poptart_Dispenser(void) {
	//On the distruction of this object, make sure that the DispensedItem has first been deleted to prevent any memory leaks
	if (!this->itemRetrieved) {
		delete this->DispensedItem;
	}
}

bool Poptart_Dispenser::insertMoney(int money) {
	//finds the correct method to use based on the current state for inserting money
	PoptartCurrentState = (PoptartState*)this->CurrentState;
	return this->PoptartCurrentState->insertMoney(money);
}

bool Poptart_Dispenser::makeSelection(int option) {
	//finds the correct method to use based on the current state for making a selection
	PoptartCurrentState = (PoptartState*)this->CurrentState;
	return this->PoptartCurrentState->makeSelection(option);
}

bool Poptart_Dispenser::moneyRejected(void) {
	//finds the correct method to use based on the current state for rejecting money
	PoptartCurrentState = (PoptartState*)this->CurrentState;
	return this->PoptartCurrentState->moneyRejected();
}

bool Poptart_Dispenser::addPoptart(int number) {
	//finds the correct method to use based on the current state for adding poptarts
	PoptartCurrentState = (PoptartState*)this->CurrentState;
	return this->PoptartCurrentState->addPoptart(number);
}

bool Poptart_Dispenser::dispense(void) {
	//finds the correct method to use based on the current state for dispensing poptarts
	PoptartCurrentState = (PoptartState*)this->CurrentState;
	return this->PoptartCurrentState->dispense();
}

Product* Poptart_Dispenser::getProduct(void) {
	//changes the bools to now match the product being retrived rather than simply dispensed. This is needed so we can delete the object now that's it has been retrieved
	if (this->itemDispensed) {
		this->itemDispensed = false;
		this->itemRetrieved = true;
		return this->DispensedItem;
	}

	return nullptr;
}

void Poptart_Dispenser::setStateParam(stateParameter SP, int value) {
	//if the pass state parameter was the cost of a popatrt then ignore this method
	if (SP == Cost_Of_Poptart) return;
	//use state parameter and value passed into the method to update the value associated with the relevant state parameter passed
	this->stateParameters[SP] = value;
}

int Poptart_Dispenser::getStateParam(stateParameter SP) {
	//if the current state parameter is the cost of a poptart then return the products cost by getting it by accessing the pointer stored in DispensedItem
	if (SP == Cost_Of_Poptart) {
		if (DispensedItem == nullptr) return 0;
		return DispensedItem->cost();
	}
	//If it's any other state parameter then simply return the value associated with the relevant state parameter
	return this->stateParameters[SP];
}
#pragma endregion

#pragma region OutOfPoptart State
bool OutOfPoptart::insertMoney(int money) {
	cout << "Error: No poptarts in the dispenser." << endl;
	return false;
}

bool OutOfPoptart::makeSelection(int option) {
	cout << "Error: No selection was made." << endl;
	return false;
}

bool OutOfPoptart::moneyRejected(void) {
	cout << "Ejecting money." << endl;

	//remove the money from the dispenser and then put it back into the no credit state
	this->CurrentContext->setStateParam(Credit, 0);

	this->CurrentContext->setState(Out_Of_Poptart);
	return true;
}

bool OutOfPoptart::addPoptart(int number) {
	cout << number << " poptarts added to the dispenser." << endl;

	//if there is no poptarts in the dispenser than it should add them with a generic value that is not specific to any one type of poptart
	this->CurrentContext->setStateParam(No_Of_Poptarts, number);

	this->CurrentContext->setState(No_Credit);
	return true;
}

bool OutOfPoptart::dispense(void) {
	cout << "Error: No poptarts left to dispense." << endl;
	return false;
}
#pragma endregion

#pragma region NoCredit State
bool NoCredit::insertMoney(int money) {
	//Insert money into the vending machine if there is currently no money in it.
	cout << "You inserted: " << money;
	this->CurrentContext->setStateParam(Credit, money);

	//output the total amount of money in the vending machine
	cout << " Total: " << money << endl;
	this->CurrentContext->setState(Has_Credit);

	return true;
}

bool NoCredit::makeSelection(int option) {
	cout << "Error: No money." << endl;
	return false;
}

bool NoCredit::moneyRejected(void) {
	cout << "Error: No money." << endl;
	return false;
}

bool NoCredit::addPoptart(int number) {
	cout << "Error: Already have poptarts." << endl;
	return false;
}

bool NoCredit::dispense(void) {
	cout << "Error: No money." << endl;
	return false;
}
#pragma endregion

#pragma region HasCredit State
bool HasCredit::insertMoney(int money) {
	//allows someone to insert more money even though there is already money in the dispenser
	cout << "You inserted: " << money;
	this->CurrentContext->setStateParam(Credit, money += this->CurrentContext->getStateParam(Credit));
	//output the total money now in the dispenser
	cout << " Total: " << this->CurrentContext->getStateParam(Credit) << endl;

	//Remains in the hasCredit state
	this->CurrentContext->setState(Has_Credit);
	return true;
}

bool HasCredit::makeSelection(int option) {
	cout << "You selected option: " << option << endl;

	//compared the option passed into makeSelection with the opcode for each type of poptart.
	//if it finds a match then create a new instance of the respective poptart.
	//Making use of the factory pattern the below switch statement is essentially the "factory" and is responsible for creating the different poptarts
	switch (option) {
		case 0x1:
			((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Plain();
			break;
		case 0x2:
			((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Spicy();
			break;
		case 0x4:
			((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Chocolate();
			break;
		case 0x8:
			((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Coconut();
			break;
		case 0x10:
			((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem = new Fruity();
			break;
		default:
			//if an invalid selection was made, return false when will return to the HasCredit state
			cout << "Invalid poptart selected." << endl;
			return false;
	}

	//sets the cost of poptart param to the cost of the selected poptart
	this->CurrentContext->setStateParam(Cost_Of_Poptart, ((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem->cost());

	//if the cost of the selected poptart is less than or equal to the money that was put into the dispenser then
	if (this->CurrentContext->getStateParam(Cost_Of_Poptart) <= this->CurrentContext->getStateParam(Credit)) {
		this->CurrentContext->setState(Dispenses_Poptart);
		return true;
	//else if there wasn't enough money
	} else {
		//return to the HasCredit state
		cout << "You do not have enough money for the selected poptart." << endl;
		return false;
	}
}

bool HasCredit::moneyRejected(void) {
	cout << "Ejecting money." << endl;

	//remove the money from the dispenser and then put them back into the no credit state
	this->CurrentContext->setStateParam(Credit, 0);

	this->CurrentContext->setState(No_Credit);
	return true;
}

bool HasCredit::addPoptart(int number) {
	cout << "Error: You have Money." << endl;
	return false;
}

bool HasCredit::dispense(void) {
	cout << "Error: You have Money." << endl;
	return false;
}
#pragma endregion

#pragma region DispensesPoptart State
bool DispensesPoptart::insertMoney(int money) {
	cout << "Error: Busy dispensing poptart." << endl;
	return false;
}

bool DispensesPoptart::makeSelection(int option) {
	cout << "Error: Busy dispensing poptart." << endl;
	return false;
}

bool DispensesPoptart::moneyRejected(void) {
	cout << "Error: Busy dispensing poptart." << endl;
	return false;
}

bool DispensesPoptart::addPoptart(int number) {
	cout << "Error: Busy dispensing poptart." << endl;
	return false;
}

bool DispensesPoptart::dispense(void) {
	//the below 2 param changes must be done before the local ints are set to ensure the local ints have the correct values
	//subtract the cost of a poptart from the total money
	this->CurrentContext->setStateParam(Credit, this->CurrentContext->getStateParam(Credit) - this->CurrentContext->getStateParam(Cost_Of_Poptart));
	//lower the number of available poptarts by 1
	this->CurrentContext->setStateParam(No_Of_Poptarts, this->CurrentContext->getStateParam(No_Of_Poptarts) - 1);

	//gets the current money and available poptarts, saves them as a local var to simplify code
	int credit = this->CurrentContext->getStateParam(Credit);
	int noOfPoptarts = this->CurrentContext->getStateParam(No_Of_Poptarts);

	//get the current poptart that will be dispensed before it's deleted
	cout << "The poptart you have dispensed is " << ((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem->description() << endl;

	if (credit > 0 && noOfPoptarts > 0) { //If there is credits and poptarts available
		cout << "You have money and there are poptarts available." << endl;
		this->CurrentContext->setState(Has_Credit);
	} else if(credit == 0 && noOfPoptarts > 0) { //elseif there is no money but poptarts are still available
		cout << "You have no money but there are poptarts available." << endl;
		this->CurrentContext->setState(No_Credit);
	} else { //else there are no remaining poptarts
		cout << "There are no poptarts left to dispense." << endl;
		this->CurrentContext->setState(Out_Of_Poptart);
	}

	//sets the itemDispensed from Poptart_Dispenser to true; this will then be used to delete the item once dispensed
	((Poptart_Dispenser*)(this->CurrentContext))->itemDispensed = true;

	//call consume which will delete the selected poptart as it's data is now 
	((Poptart_Dispenser*)(this->CurrentContext))->DispensedItem->consume();

	return true;
}
#pragma endregion