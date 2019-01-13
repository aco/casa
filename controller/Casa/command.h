#pragma once

typedef enum
{
<<<<<<< HEAD
	CMDNODE = 0,
	CMDREPORT = 1,
	CMDNOTIFICATION = 2,
=======
	/// <summary>
	/// A node update request.
	/// </summary>
	CMDNODE = 0,
	/// <summary>
	/// An ambient/system report.
	/// </summary>
	CMDREPORT = 1,
	/// <summary>
	/// A notification to dispatch.
	/// </summary>
	CMDNOTIFICATION = 2,
	/// <summary>
	/// A confirmation of receipt.
	/// </summary>
>>>>>>> 74d36b798d227c747c4e05ccce04a0f342b36110
	CMDCONFIRMATION = 3
} Command;