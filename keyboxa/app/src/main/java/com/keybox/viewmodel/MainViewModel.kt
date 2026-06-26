package com.keybox.viewmodel

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import com.keybox.Entry
import com.keybox.Group

class MainViewModel : ViewModel() {
    private val _entries = MutableLiveData<List<Entry>>(emptyList())
    val entries: LiveData<List<Entry>> = _entries

    private val _groups = MutableLiveData<List<Group>>(emptyList())
    val groups: LiveData<List<Group>> = _groups

    private val _selectedGroup = MutableLiveData<Group?>(null)
    val selectedGroup: LiveData<Group?> = _selectedGroup

    private val _isFileLoaded = MutableLiveData<Boolean>(false)
    val isFileLoaded: LiveData<Boolean> = _isFileLoaded

    private val _isUnlocked = MutableLiveData<Boolean>(false)
    val isUnlocked: LiveData<Boolean> = _isUnlocked

    private val _pendingNewFile = MutableLiveData<Boolean>(false)
    val pendingNewFile: LiveData<Boolean> = _pendingNewFile

    private val _loadError = MutableLiveData<String?>(null)
    val loadError: LiveData<String?> = _loadError

    fun setEntries(newEntries: List<Entry>) {
        _entries.value = newEntries
    }

    fun setGroups(newGroups: List<Group>) {
        _groups.value = newGroups
    }

    fun setSelectedGroup(group: Group?) {
        _selectedGroup.value = group
    }

    fun setFileLoaded(loaded: Boolean) {
        _isFileLoaded.value = loaded
    }

    fun setUnlocked(unlocked: Boolean) {
        _isUnlocked.value = unlocked
    }

    fun setPendingNewFile(pending: Boolean) {
        _pendingNewFile.value = pending
    }

    fun setLoadError(error: String?) {
        _loadError.value = error
    }
}
