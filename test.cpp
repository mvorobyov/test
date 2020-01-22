#include <iostream>
#include <string>
#include <set>
#include <list>
#include <algorithm>
#include <functional>
#include <iterator>

using namespace std;

template<typename T>
struct is_container
{
    static constexpr bool value = false;
};

template<typename T>
struct is_container<list<T>>
{
    static constexpr bool value = true;
};

template<typename T>
struct is_container<set<T>>
{
    static constexpr bool value = true;
};

template<class C, class = typename std::enable_if<is_container<C>::value>::type>
std::ostream & operator<<(std::ostream & os, const C & container)
{
    os << "{ ";
    // for_each(begin(container), end(container), [&os](const typename C::value_type & v){ os << v << " "; });
    copy(container.cbegin(), container.cend(), ostream_iterator<typename C::value_type>(os, " "));
    return os << "}";
}

using tag_register = set<string>;

class Note
{
public:
    Note(const string & title, const string & text, const tag_register & tags) : title(title), text(text), tags(tags) {};
    const string & getTitle() const { return title; };
    const string & getText() const { return text; };
    const tag_register & getTags() const { return tags; };
    bool operator==(const Note & rhs) const { return title == rhs.title && text == rhs.text && tags == rhs.tags; };
    friend ostream & operator<<(ostream & os, const Note & note) { return os << "{ " << note.title << ", " << note.text << ", " << note.tags << " }"; };
private:
	string title;
	string text;
	tag_register tags;
};

using note_register = list<Note>;

class Storyboard
{
public:
	void addNote(const Note & note) { notes.push_back(note); };
	void deleteNote(const Note & note) { notes.remove(note); };
	size_t getSize() const { return notes.size(); };
	const note_register searchByTitle(const string & title) const { return findAllNotesIf([title](const Note & note) { return note.getTitle() == title; }); };
	const note_register searchByText(const string & text) const { return findAllNotesIf([text](const Note & note) { return note.getText() == text; }); };
	const note_register searchByTags(const tag_register & tags) const { return findAllNotesIf([tags](const Note & note) { return includes(note.getTags().cbegin(), note.getTags().cend(), tags.cbegin(), tags.cend()); }); };
    friend ostream & operator<<(ostream & os, const Storyboard & board) { return os << board.notes; };
private:
    const note_register findAllNotesIf(function<bool(const Note &)>) const;
private:
	note_register notes;
};

const note_register Storyboard::findAllNotesIf(function<bool(const Note &)> pred) const
{
    note_register reg;
    auto it = find_if(notes.cbegin(), notes.cend(), pred);
    while( it != notes.cend() )
    {
        reg.push_back(*it);
        it = find_if(++it, notes.cend(), pred);
    }
    return reg;
}

const string status(const bool success) { return success ? "passed" : "FAILED!!!"; }

void checkBoardSize(const Storyboard & board, const size_t size) { cout << "checkBoardSize(" << size << "): " << status(board.getSize() == size) << " : " << board << "\n"; }

void checkByTitle(const Storyboard & board, const string & title, const size_t size)
{
    const auto notes = board.searchByTitle(title);
    cout << "checkByTitle(" << title << ", " << size << "): " << status(notes.size() == size) << " : " << notes << "\n";
}

void checkByText(const Storyboard & board, const string & text, const size_t size)
{
    const auto notes = board.searchByText(text);
    cout << "checkByText(" << text << ", " << size << "): " << status(notes.size() == size) << " : " << notes << "\n";
}

void checkByTags(const Storyboard & board, const tag_register & tags, const size_t size)
{
    const auto notes = board.searchByTags(tags);
    cout << "checkByTags(" << tags << ", " << size << "): " << status(notes.size() == size) << " : " << notes << "\n";
}

int main()
{
    Storyboard sb;
    Note note1("title1", "text1", tag_register{"tag1", "tag2"});
    Note note2("title2", "text2", tag_register{"tag2", "tag3"});
    Note note3("title3", "text1", tag_register{"tag1", "tag3"});
    
    cout << "adding note1 and note2" << endl;
    sb.addNote(note1);
    sb.addNote(note2);
    checkBoardSize(sb, 2);
    
    cout << "adding note1 again and note3" << endl;
    sb.addNote(note1);
    sb.addNote(note3);
    checkBoardSize(sb, 4);

    checkByTitle(sb, note1.getTitle(), 2);
    checkByTitle(sb, note2.getTitle(), 1);
    checkByTitle(sb, note3.getTitle(), 1);
    checkByTitle(sb, "title4", 0);

    checkByText(sb, note1.getText(), 3);
    checkByText(sb, note2.getText(), 1);
    checkByText(sb, note3.getText(), 3);
    checkByText(sb, "text4", 0);

    checkByTags(sb, note1.getTags(), 2);
    checkByTags(sb, note2.getTags(), 1);
    checkByTags(sb, note3.getTags(), 1);
    checkByTags(sb, {"tag1"}, 3);
    checkByTags(sb, {"tag4"}, 0);

    return 0;
}
